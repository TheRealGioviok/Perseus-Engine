#include "Game.h"
#include "tables.h"
#include "constants.h"
#include "history.h"
#include "zobrist.h"
#include "evaluation.h"
#include "uci.h"
#include "tt.h"
#include <iostream>
#include <cstdlib>

// Global var to debug the SE implementation
U64 seCandidates = 0;
U64 seActivations = 0;
U64 avgDist = 0;


static inline Score mateIn(Ply ply) { return ((mateScore) - (ply)); }
static inline Score matedIn(Ply ply) { return ((-mateScore) + (ply)); }
static inline Score randomizedDrawScore(U64 nodes) { return 4 - (nodes & 7); }
static inline Score adjustMateScore(Score score, Ply ply) { return score + ply * (score < -mateValue) - ply * (score > mateValue); }

inline bool okToReducePacked(Position &pos, PackedMove move)
{
    return !((movePiece(move) != NOPIECE) | (pos.pieceOn(moveTarget(move)) != NOPIECE));
}

static inline Depth reduction(Depth d, U16 m, bool isQuiet, bool isPv, bool improving)
{
    return reductionTable[isQuiet][std::min((int)d,64)][std::min((int)m,64)] - isPv - improving;
}

static inline Score futilityMargin(Depth depth, bool improving)
{
    return futilityMarginDelta * (depth - improving);
}

static inline int sortTTUp(MoveList &ml, PackedMove ttMove)
{
    for (int i = 0; i < ml.count; i++)
    {
        if (packedMoveCompare(ttMove, (Move)ml.moves[i]))
        {
            ml.moves[0] = ml.moves[i];
            ml.moves[i] = 0;
            return 0;
        }
    }
    return 1;
}

static inline void updateKillers(SStack* ss, Move killerMove)
{
    if (ss->killers[0] != killerMove)
    {
        ss->killers[1] = ss->killers[0];
        ss->killers[0] = killerMove;
    }
}

static inline void updateCounters(Move counter, Move lastMove)
{
    if (lastMove) counterMoveTable[indexFromTo(moveSource(lastMove), moveTarget(lastMove))] = counter;
}

static inline void clearKillers(SStack *ss)
{
    ss->killers[0] = noMove;
    ss->killers[1] = noMove;
}

static inline void clearExcludedMove(SStack *ss)
{
    ss->excludedMove = noMove;
}

Score Game::search(Score alpha, Score beta, Depth depth, const bool cutNode, SStack *ss)
{
    // Comms
    if (stopped)
        return 0;
    if ((nodes & comfrequency) == 0)
    {
        communicate();
        if (stopped)
            return 0;
    }
    assert(pos.hashKey == pos.generateHashKey());
    // Ply overflow
    if (ply >= maxPly)
        return evaluate();

    const bool RootNode = ply == 0;
    const bool PVNode = (beta - alpha) > 1;

    Score eval;
    bool improving = true;
    // const Move excludedMove = ss->excludedMove;
    // Guard from pvlen editing when in singular extension
    // if (!excludedMove) 
        pvLen[ply] = ply;

    // Update seldepth
    seldepth = std::max(Ply(seldepth), Ply(ply + 1));

    bool inCheck = isCheck(pos.lastMove) || pos.inCheck();

    // Draws - mate distance
    if (!RootNode)
    {
        if (isRepetition() || pos.fiftyMove >= 100 || pos.insufficientMaterial())
            return randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

        // Mate distance pruning
        alpha = std::max(alpha, matedIn(ply));
        beta = std::min(beta, mateIn(ply + 1));
        if (alpha >= beta)
            return alpha;
    }

    //// TT probe
    ttEntry *tte = probeTT(pos.hashKey); //excludedMove ? nullptr : probeTT(pos.hashKey);
    const bool ttHit = tte;
    const Score ttScore = ttHit ? adjustMateScore(tte->score, ply) : noScore;
    const PackedMove ttMove = ttHit ? tte->bestMove : 0;
    const U8 ttFlags = ttHit ? tte->flags : 0;
    const U8 ttBound = ttFlags & 3;
    const Depth ttDepth = ttHit ? tte->depth : 0;

    if (ttScore != noScore)
    {
        if (!PVNode && ttDepth >= depth)
        {
            if (ttBound == hashEXACT)
                return ttScore;
            if ((ttBound == hashLOWER))
                alpha = std::max(alpha, ttScore);
            else if ((ttBound == hashUPPER))
                beta = std::min(beta, ttScore);
            if (alpha >= beta) {
                // Update best move history
                // Square from = moveSource(ttMove);
                // Square to = moveTarget(ttMove);
                // updateHistoryBonus(&historyTable[pos.side][indexFromTo(from, to)], depth, true);
                return ttScore;
            }
        }
    }

    const bool ttPv = PVNode || (ttFlags & hashPVMove);

    // Clear killers
    clearKillers(ss+1);
    // Clear excluded move
    (ss+1)->excludedMove = noMove;

    if (inCheck && (depth <= 0 || ply < currSearch * (1 + PVNode)))
        depth++;

    // Quiescence drop
    if (depth <= 0) return quiescence(alpha, beta, ss);

    // else if (depth < ttDepth && (ttBound & hashLOWER)){
    //     if (ply == 1) depth += std::min(3, ttDepth - depth);
    //     else depth++; // Principled iterative extensions: If the tt entry is a lower bound (or exact), we don't search shallower than the tt depth
    // }
    // Initialize the undoer
    UndoInfo undoer = UndoInfo(pos);

    if (inCheck)
    {
        ss->staticEval = eval = noScore;
        // improving = false;
        goto skipPruning;
    }
    

    // Get static eval of the position
    if (ttHit)
    {
        // Check if the eval is stored in the TT
        eval = ss->staticEval = tte->eval != noScore ? tte->eval : evaluate();
        // Also, we might be able to use the score as a better eval
        if (ttScore != noScore && (ttBound == hashEXACT || (ttBound == hashUPPER && ttScore < eval) || (ttBound == hashLOWER && ttScore > eval)))
            eval = ttScore;
    }
    // else if (excludedMove){
    //     eval = ss->staticEval; // We already have the eval from the main search in the current ss entry
    // }
    else {
        eval = ss->staticEval = evaluate();
        // Store the eval in the TT if not in exclusion mode (in which we might already have the entry)
        writeTT(pos.hashKey, noScore, eval, 0, hashNONE, 0, ply, PVNode, ttPv);
    }

    // // Calculate the improving flag
    if ((ss - 2)->staticEval != noScore)
        improving = ss->staticEval > (ss - 2)->staticEval;
    else if ((ss - 4)->staticEval != noScore)
        improving = ss->staticEval > (ss - 4)->staticEval;

    // Pruning time
    if (!PVNode) // && !excludedMove)
    {
        // RFP
        if (depth <= RFPDepth && abs(eval) < mateScore && eval - futilityMargin(depth, improving) >= beta) // && !excludedMove)
            return eval;

        // Null move pruning
        if (eval >= ss->staticEval &&
            eval >= beta &&
            (ss - 1)->move != noMove &&
            depth >= 3 &&
            okToReduce(pos.lastMove) && pos.hasNonPawns() &&
            ply >= nmpPlies)
        {

            // make null move
            makeNullMove();
            ss->move = noMove;
            ss->contHistEntry = continuationHistoryTable[0];

            Depth R = nmpQ1 + (depth / nmpDepthDivisor) + std::min((eval - beta) / nmpScoreDivisor, nmpQ2);
            Score nullScore = -search(-beta, -beta + 1, depth - R, !cutNode, ss + 1);

            undoNullMove(undoer);

            if (stopped)
                return 0;

            if (nullScore >= beta)
            {
                if (nullScore >= mateValue)
                    nullScore = beta;
                if (nmpPlies || depth <= 14)
                    return nullScore;

                // If the null move failed high, we can try a reduced search when depth is high. This is to avoid zugzwang positions
                nmpPlies = ply + (depth - R) * 2 / 3;
                Score verification = search(beta - 1, beta, depth - R, false, ss);
                nmpPlies = 0;

                if (verification >= beta)
                    return nullScore;
            }
        }
    
        // Razoring
        if (depth <= razorDepth && abs(eval) < mateScore && eval + razorQ1 + depth * razorQ2 < alpha)
        {
            const Score razorScore = quiescence(alpha, beta, ss);
            if (razorScore <= alpha) return razorScore;
        }
    
    }

skipPruning:

    if (depth >= IIRdepth && ttBound == hashNONE)
        --depth; // && !excludedMove

    // Search
    const Score origAlpha = alpha;
    Score bestScore = noScore;
    Move bestMove = noMove;
    U16 moveSearched = 0;

    Move quiets[256], noisy[256];
    U16 quietsCount = 0, noisyCount = 0;

    MoveList moveList;
    generateMoves(moveList, ss);

    // Iterate through moves
    for (int i = (ttMove ? sortTTUp(moveList, ttMove) : 1); i < moveList.count; i++) // Slot 0 is reserved for the tt move, wheter it is present or not
    {
        
        Move currMove = onlyMove(moveList.moves[i]);
        // assert (
        //     i != 0 || !excludedMove ||
        //     (excludedMove == currMove)
        // );
        if (!currMove) continue; // || currMove == excludedMove

        const bool isQuiet = okToReduce(currMove);
        // const bool givesCheck = isCheck(currMove) || pos.inCheck();
        
        if (makeMove(currMove))
        {
            U64 nodesBefore = nodes;
            // // Singular extension
            // Depth extension = 0;
            // if (i == 0 // Can only happen on ttMove
            //     && ply < currSearch * 2  
            //     && !RootNode 
            //     && depth >= singularSearchDepth
            //     && !excludedMove 
            //     && (ttBound & hashLOWER) 
            //     && abs(ttScore) < mateValue 
            //     && ttDepth >= depth - 3
            // ){
            //     // Increase singular candidates
            //     ++seCandidates;
            //     const Score singularBeta = ttScore - singularDepthMultiplier * depth;
            //     const Depth singularDepth = (depth - 1) / 2;

            //     ss->excludedMove = currMove;
            //     const Score singularScore = search(singularBeta - 1, singularBeta, singularDepth, cutNode, ss);
            //     ss->excludedMove = noMove;

            //     if (singularScore < singularBeta) {
            //         extension = 1;
            //         // Increase singular activations
            //         ++seActivations;
            //     }
            //     // else{
            //     //     std::cout << "info string Singular failed with score: " << singularScore << " beta: " << singularBeta << std::endl;
            //     // }

            //     // Update avg dist
            //     avgDist += abs(singularScore - singularBeta);
            // }

            Depth newDepth = depth - 1; // + extension;

            ss->move = currMove;
            ss->contHistEntry = continuationHistoryTable[indexPieceTo(movePiece(currMove), moveTarget(currMove))];
            S32 currMoveScore = getScore(moveList.moves[i]); // - BADNOISYMOVE;
            Score score;
        
            ++nodes;
            // bool givesCheck = isCheck(currMove) || pos.inCheck();
            if (isQuiet) quiets[quietsCount++] = currMove;
            else noisy[noisyCount++] = currMove;
            if (RootNode && depth >= LOGROOTMOVEDEPTH) std::cout << "info depth " << std::dec << (int)currSearch << " currmove " << getMoveString(currMove) << " currmovenumber " << moveSearched + 1 << " currmovescore " << currMoveScore << " hashfull " << hashfull() << std::endl; // << " kCoffs: " << kCoffs << "/" << kEncounters << std::endl;

            if (moveSearched > PVNode * 3 && depth >= 3 && (isQuiet || !ttPv))
            {
                Depth R = reduction(depth, moveSearched, isQuiet, ttPv, improving);
                if (currMoveScore >= COUNTERSCORE) R -= 1;
                if (isQuiet){
                    // R -= givesCheck;
                    R -= (S8)std::clamp((currMoveScore - QUIETSCORE) / 8192LL, -2LL, 2LL);
                    if (cutNode) R += 2;
                    if (ttPv) R -= cutNode;
                }
                else {
                    if (currMoveScore < GOODNOISYMOVE) {
                        if (cutNode) R += 1;
                        R -= (S8)std::clamp((currMoveScore - BADNOISYMOVE) / 6144LL, -1LL, 2LL);
                    }
                    R -= (S8)std::clamp((currMoveScore - GOODNOISYMOVE - BADNOISYMOVE) / 6144LL, -1LL, 2LL);
                }
                R = std::max(Depth(0), R);
                R = std::min(Depth(newDepth - Depth(1)), R);
                

                Depth reducedDepth = newDepth - R;
                // Search at reduced depth
                score = -search(-alpha - 1, -alpha, reducedDepth, true, ss + 1);
                // If failed high on reduced search, research at full depth
                if (score > alpha && R)
                    score = -search(-alpha - 1, -alpha, newDepth, !cutNode, ss + 1);
            }
            else 
                if (!PVNode || moveSearched)
                    score = -search(-alpha - 1, -alpha, newDepth, !cutNode, ss + 1);

            // Pvsearch
            if (PVNode && (!moveSearched || score > alpha))
                score = -search(-beta, -alpha, newDepth, false, ss + 1);
            
            undo(undoer, currMove);

            if (RootNode) nodesPerMoveTable[indexFromTo(moveSource(currMove), moveTarget(currMove))] += nodes - nodesBefore;

            ++moveSearched;

            if (score > bestScore) {
                bestScore = score;
                if (score > alpha) {
                    bestMove = currMove;
                    
                    if (PVNode) {
                        pvTable[ply][ply] = currMove;
                        if (ply + 1 < maxPly) {
                            // Copy the pv from the next ply
                            for (int j = ply + 1; j < pvLen[ply + 1]; j++)
                                pvTable[ply][j] = pvTable[ply + 1][j];
                            pvLen[ply] = pvLen[ply + 1];
                        }
                    }
                    if (score >= beta) {
                        if (isQuiet) {
                            updateKillers(ss, currMove);
                            updateCounters(currMove, (ss - 1)->move);
                        }
                        updateHH(ss, pos.side, depth, currMove, quiets, quietsCount, noisy, noisyCount);
                        break;
                    }
                    alpha = score;
                }
            }
        }
        else
            undo(undoer, currMove);
    }

    //// Check for checkmate /
    if (moveSearched == 0){
        // return excludedMove ? alpha : (inCheck ? matedIn(ply) : randomizedDrawScore(nodes)); // Randomize draw score so that we try to explore different lines
        return inCheck ? matedIn(ply) : randomizedDrawScore(nodes);
    }
        // 
        // return inCheck ? matedIn(ply) : randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

    if (!stopped){ // && !excludedMove){
        U8 ttStoreFlag = bestScore >= beta ? hashLOWER : alpha != origAlpha ? hashEXACT : hashUPPER;
        writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply, PVNode, ttPv);
        // writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply, false, PVNode);
    }

    return bestScore;
}

Score Game::quiescence(Score alpha, Score beta, SStack *ss)
{
    bool inCheck = pos.inCheck();
    if (inCheck)
        if (isRepetition())
            return randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

    Score bestScore;

    if (ply >= maxPly - 1)
        return inCheck ? 0 : evaluate();

    const bool PVNode = (beta - alpha) > 1; 
    
     //// TT probe
    ttEntry *tte = probeTT(pos.hashKey); //excludedMove ? nullptr : probeTT(pos.hashKey);
    const bool ttHit = tte;
    const Score ttScore = ttHit ? adjustMateScore(tte->score, ply) : noScore;
    const PackedMove ttMove = ttHit ? tte->bestMove : 0;
    const U8 ttFlags = ttHit ? tte->flags : 0;
    const U8 ttBound = ttFlags & 3;

    if (ttScore != noScore)
    {
        if (!PVNode)
        {
            if (ttBound == hashEXACT)
                return ttScore;
            if ((ttBound == hashLOWER))
                alpha = std::max(alpha, ttScore);
            else if ((ttBound == hashUPPER))
                beta = std::min(beta, ttScore);
            if (alpha >= beta) {
                // Update best move history
                // Square from = moveSource(ttMove);
                // Square to = moveTarget(ttMove);
                // updateHistoryBonus(&historyTable[pos.side][indexFromTo(from, to)], depth, true);
                return ttScore;
            }
        }
    }

    const bool ttPv = PVNode || (ttFlags & hashPVMove); 

    if (inCheck){
        bestScore = noScore;
        ss->staticEval = noScore;
    }
    else if (ttHit){
        ss->staticEval = bestScore = tte->eval != noScore ? tte->eval : evaluate();
        if (ttScore != noScore && (ttBound == hashEXACT || (ttBound == hashUPPER && ttScore < bestScore) || (ttBound == hashLOWER && ttScore > bestScore)))
            bestScore = ttScore;
    }
    else {
        ss->staticEval = bestScore = evaluate();
        writeTT(pos.hashKey, noScore, bestScore, 0, hashNONE, 0, ply, PVNode, ttPv);
    }

    if (bestScore >= beta)
        return beta;
    alpha = std::max(alpha, bestScore);

    // Generate moves
    MoveList moveList;

    inCheck ? generateMoves(moveList, ss) : generateCaptures(moveList);

    UndoInfo undoer = UndoInfo(pos);
    U16 moveCount = 0;
    Move bestMove = noMove;
    for (int i = sortTTUp(moveList, ttMove); i < moveList.count; i++)
    {
        Move move = onlyMove(moveList.moves[i]);
        
        // SEE pruning : skip all moves that have see < -100 (We may want to do this with a threshold of 0, but we would introduce another see call. TODO: lazily evaluate the see so that we can skip moves with see < 0)
        if (!inCheck && moveCount && getScore(moveList.moves[i]) < GOODNOISYMOVE)
            break;
        if (makeMove(move))
        {
            ss->move = move;
            ss->contHistEntry = continuationHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))];
            moveCount++;
            Score score = -quiescence(-beta, -alpha, ss+1);
            undo(undoer, move);
            if (score > bestScore)
            {
                bestScore = score;
                if (score > alpha)
                {
                    bestMove = move;
                    if (score >= beta)
                        break;
                    alpha = score;
                }
            }
        }
        else
            undo(undoer, move);
    }

    if (moveCount == 0 && inCheck)
        return matedIn(ply);
    
    U8 ttStoreFlag = bestScore >= beta ? hashLOWER : hashUPPER; // No exact bounds in qsearch
    ttStoreFlag |= ttPv ? hashPVMove : 0;
    writeTT(pos.hashKey, bestScore, ss->staticEval, 0, ttStoreFlag, bestMove, ply, PVNode, ttPv);

    return bestScore;
}

#define ASPIRATIONWINDOW 25
void Game::startSearch(bool halveTT = true)
{
    // Set SE counters to 0
    seCandidates = 0;
    seActivations = 0;
    avgDist = 0;
    SStack stack[4 + maxPly + 1]; // 4 is the max ply that we look back, 1 is the max ply that we look forward (for now it is actually -2, but hopefully we extend conthist to -4)
    SStack* ss = stack + 4;

    for (int i = -4; i < maxPly + 1; i++) (ss+i)->wipe();

    Score delta = ASPIRATIONWINDOW;
    nodes = 0ULL;
    stopped = false;
    ply = 0;
    seldepth = 0;
    nmpPlies = 0;

    std::cin.clear();

    pos.lastMove = 0;
    lastScore = 0;

    // Clear nodesPerMoveTable
    

    // killerCutoffs = 0;
    // posKillerSearches = 0;

    startTime = getTime64();
    U64 optim = 0xffffffffffffffffULL;
    switch (searchMode)
    {
    case 0: case 3:               // Infinite search
        depth = maxPly - 1; // Avoid overflow
    case 1: // Fixed depth
        optim = moveTime = 0xffffffffffffffffULL;
        break;
#define UCILATENCYMS 20
    case 2: // Time control
        moveTime = getTime64();
        U64 totalTime;
        if (pos.side == WHITE) totalTime = (U64)(wtime / 10) + winc;
        else totalTime = (U64)(btime / 10) + binc;
        totalTime -= UCILATENCYMS;
        optim = totalTime / 4;

        moveTime += totalTime;
        depth = maxPly - 1;
        break;
    }

    double nodesTmScale = 1.0;

    // Clear nodePerMoveTable
    memset(nodesPerMoveTable, 0, sizeof(nodesPerMoveTable));

    // Clear pv len and pv table
    memset(pvLen, 0, sizeof(pvLen));
    memset(pvTable, 0, sizeof(pvTable));

    Score alpha = noScore;
    Score beta = infinity;

    // Always compute a depth 1 search
    rootDelta = 2 * infinity;
    currSearch = 1;

    Score score = search(noScore, infinity, 1, false, ss);
    Move bestMove = pvTable[0][0];

    std::cout << "info score depth 1 cp " << score << " nodes " << nodes << " moves ";
    printMove(bestMove);
    std::cout << std::endl;
    depth = std::min(depth, Depth(maxPly - 3));
    if (depth < 0)
        depth = maxPly - 3;
    if (stopped)
        goto bmove;

    for (currSearch = 2; (currSearch <= depth) && currSearch >= 2 && !stopped; currSearch++)
    {
        if (currSearch >= 4)
        {
            alpha = std::max(S32(noScore), score - delta);
            beta = std::min(S32(infinity), score + delta);
            
        }
        // Calculate optim base
        while (true)
        {
            seldepth = 0; // Reset seldepth
            ply = 0;
            S64 locNodes = nodes;                        // Save nodes for nps calculation
            U64 timer1 = getTime64();                    // Save time for nps calculation
            // Clear the first sstack entry
            ss->wipe();
            score = search(alpha, beta, currSearch, false, ss); // Search at depth currSearch
            if (stopped)
                goto bmove;
            bestMove = pvTable[0][0];
            U64 timer2 = getTime64();

            if (score <= alpha)
            {
                beta = (alpha + beta) / 2;
                alpha = std::max(S32(noScore), score - delta);

                if (score < -mateValue && score > -mateScore)
                    std::cout << std::dec << "info depth " << (int)currSearch << " score mate " << -(mateScore + score + 2) / 2 << " lowerbound nodes " << nodes << " pv ";
                else if (score > mateValue && score < mateScore)
                    std::cout << std::dec << "info depth " << (int)currSearch << " score mate " << (mateScore + 1 - score) / 2 << " lowerbound nodes " << nodes << " pv ";
                else
                    std::cout << std::dec << "info depth " << (int)currSearch << " score cp " << (score >> 1) << " lowerbound nodes " << nodes << " pv ";
                printMove(pvTable[0][0]);
                std::cout << " nps " << (((nodes - locNodes) * 1000) / (timer2 - timer1 + 1)) << std::endl;
                delta *= 1.44;
            }
            else if (score >= beta)
            {
                beta = std::min(S32(infinity), score + delta);
                if (score < -mateValue && score > -mateScore)
                    std::cout << std::dec << "info depth " << (int)currSearch << " score mate " << -(mateScore + score + 2) / 2 << " upperbound nodes " << nodes << " pv ";
                else if (score > mateValue && score < mateScore)
                    std::cout << std::dec << "info depth " << (int)currSearch << " score mate " << (mateScore + 1 - score) / 2 << " upperbound nodes " << nodes << " pv ";
                else
                    std::cout << std::dec << "info depth " << (int)currSearch << " score cp " << (score >> 1) << " upperbound nodes " << nodes << " pv ";
                printMove(pvTable[0][0]);
                std::cout << " nps " << (((nodes - locNodes) * 1000) / (timer2 - timer1 + 1)) << std::endl;
                delta *= 1.44;
            }
            else
            {
                if (score < -mateValue && score > -mateScore)
                    std::cout << std::dec << "info score mate " << -(mateScore + score + 2) / 2 << " depth " << (int)currSearch << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";
                else if (score > mateValue && score < mateScore)
                    std::cout << std::dec << "info score mate " << (mateScore + 1 - score) / 2 << " depth " << (int)currSearch << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";
                else
                    std::cout << std::dec << "info score cp " << (score >> 1) << " depth " << (int)currSearch << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";

                for (int i = 0; i < pvLen[0]; i++)
                {
                    printMove(pvTable[0][i]);
                    std::cout << " ";
                }
                std::cout << " nps " << (((nodes - locNodes) * 1000) / (timer2 - timer1 + 1)) << std::endl;
                break;
            }
        }
        if (currSearch >= 6){
            // Percentage ( 0.665124 ) calculated with bench @22

            nodesTmScale = 1.5 - ((double)nodesPerMoveTable[indexFromTo(moveSource(bestMove), moveTarget(bestMove))] / (double)nodes) * 0.731988329;
        }
        // Check optim time quit
        if (getTime64() > startTime + optim * nodesTmScale) break;
    }

bmove:
    // Report SE stats
    std::cout << "info string SE candidates count: " << seCandidates << " with activations: " << seActivations << "total nodes: " << nodes << std::endl;
    // Report Avg dist over all the SE candidates
    if (seCandidates)
        std::cout << "info string Avg dist: " << avgDist / seCandidates << std::endl;
    std::cout << "bestmove ";
    printMove(bestMove);
    std::cout << std::endl;
    if (halveTT)
    {
        // Age pv table
        for (U64 i = 0; i < ttEntryCount; i++)
        {
            tt[i].depth = std::max(Depth(0), Depth(tt[i].depth - Depth(2)));
            tt[i].flags |= hashOLD;
        }
    }
}