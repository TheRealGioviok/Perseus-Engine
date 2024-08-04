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

static inline Score mateIn(Ply ply) { return ((mateScore) - (ply)); }
static inline Score matedIn(Ply ply) { return ((-mateScore) + (ply)); }
static inline Score randomizedDrawScore(U64 nodes) { return 4 - (nodes & 7); }
static inline Score adjustMateScore(Score score, Ply ply) { return score + ply * (score < -mateValue) - ply * (score > mateValue); }

inline bool okToReducePacked(Position &pos, PackedMove move)
{
    return !((movePiece(move) != NOPIECE) | (pos.pieceOn(moveTarget(move)) != NOPIECE));
}

static inline Depth reduction(Depth d, U16 m, bool isPv, bool improving)
{
    return reductionTable[d][m] - isPv - improving;
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
    //if (!excludedMove) 
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
    const U8 ttFlags = ttHit ? tte->flags : hashNONE;
    const U8 ttBound = ttFlags & 3;

    if (ttScore != noScore)
    {
        if (!PVNode && tte->depth >= depth)
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

    // const bool ttPv = PVNode || (ttFlags & hashPVMove);

    // Clear killers
    clearKillers(ss+1);
    // Clear excluded move
    // (ss+1)->excludedMove = noMove;

    // Quiescence drop
    if (depth <= 0) return quiescence(alpha, beta);

    if (ply && depth >= IIRdepth && ttBound == hashNONE) --depth;

    // Initialize the undoer
    UndoInfo undoer = UndoInfo(pos);

    if (inCheck)
    {
        ss->staticEval = eval = noScore;
        // improving = false;
        goto skipPruning;
    }
    // else if (excludedMove){
    //     eval = ss->staticEval; // We already have the eval from the main search in the current ss entry
    // }

    // Get static eval of the position
    if (ttHit)
    {
        // Check if the eval is stored in the TT
        eval = ss->staticEval = tte->eval != noScore ? tte->eval : evaluate();
        // Also, we might be able to use the score as a better eval
        if (ttScore != noScore && (ttBound == hashEXACT || (ttBound == hashUPPER && ttScore < eval) || (ttBound == hashLOWER && ttScore > eval)))
            eval = ttScore;
    }
    else
    {
        eval = ss->staticEval = evaluate();
        // Store the eval in the TT if not in exclusion mode (in which we might already have the entry)
        writeTT(pos.hashKey, noScore, eval, 0, hashNONE, 0, ply, false);
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
        if (depth <= RFPDepth && abs(eval) < mateScore && eval - futilityMargin(depth, improving) >= beta)
            return eval;

        // Null move pruning
        if (ss->staticEval >= beta + nmpBias &&
            eval >= beta &&
            (ss - 1)->move != noMove &&
            depth >= 3 &&
            okToReduce(pos.lastMove) && pos.hasNonPawns() &&
            ply >= nmpPlies)
        {

            // make null move
            makeNullMove();
            ss->move = noMove;

            Depth R = nmpQ1 + (depth / nmpDepthDivisor) + std::min((eval - beta) / nmpScoreDivisor, nmpQ2);
            Score nullScore = -search(-beta, -beta + 1, depth - R, !cutNode, ss + 1);

            undoNullMove(undoer);

            if (stopped)
                return 0;

            if (nullScore >= beta)
            {
                if (nullScore >= mateValue)
                    nullScore = beta;
                if (depth <= 14)
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
            const Score razorScore = quiescence(alpha, beta);
            if (razorScore <= alpha) return razorScore;
        }
    
    }

skipPruning:

    // Search
    const Score origAlpha = alpha;
    Score bestScore = noScore;
    Move bestMove = noMove;
    U16 moveSearched = 0;

    Move quiets[256], noisy[256];
    U16 quietsCount = 0, noisyCount = 0;

    MoveList moveList;
    const Move lastMove = (ss - 1)->move;
    const Move counterMove = lastMove ? counterMoveTable[indexFromTo(moveSource(lastMove), moveTarget(lastMove))] : noMove;
    generateMoves(moveList, ss->killers[0], ss->killers[1], counterMove);

    // Iterate through moves
    for (int i = (ttMove ? sortTTUp(moveList, ttMove) : 1); i < moveList.count; i++) // Slot 0 is reserved for the tt move, wheter it is present or not
    {
        Move currMove = onlyMove(moveList.moves[i]);
        if (!currMove) continue; //  == excludedMove

        const bool isQuiet = okToReduce(currMove);
        // const bool givesCheck = isCheck(currMove) || pos.inCheck();

        // Depth extension = 0;
        // if (ply < currSearch * 2  && !RootNode && depth >= 7 && i == 0 && !excludedMove && (ttBound & hashLOWER) && abs(ttScore) < mateValue && tte->depth >= depth - 3){
        //     const Score singularBeta = ttScore - singularDepthMultiplier * depth;
        //     const Depth singularDepth = (depth - 1) / 2;

        //     ss->excludedMove = ttMove;
        //     const Score singularScore = search(singularBeta - 1, singularBeta, singularDepth, ss);
        //     ss->excludedMove = noMove;

        //     if (singularScore < singularBeta)
        //     {
        //         extension = 1;
        //         // Double extensions (todo later)
        //         if (!PVNode && singularScore < singularBeta - 15 && ss->doubleExtensions <= 2 + currSearch / 2)
        //         {
        //             extension = 2; // + (okToReduce(currMove) && singularScore < singularBeta - 100);
        //             ss->doubleExtensions = (ss - 1)->doubleExtensions + 1;
        //             // depth += depth < 10; // ?????
        //         }
        //     }
        //     if (singularBeta >= beta)
        //         return singularBeta;
        //     // Negative extensions (todo later)
        //     else if (ttScore >= beta)
        //         extension = -1 - !PVNode; // Negative extension (apparently good)
            
        // }

        Depth newDepth = depth - 1;

        ss->move = currMove;
        if (makeMove(currMove))
        {
            S32 currMoveScore = getScore(moveList.moves[i]); // - BADNOISYMOVE;
            Score score;
        
            ++nodes;
            // bool givesCheck = isCheck(currMove) || pos.inCheck();
            if (isQuiet) quiets[quietsCount++] = currMove;
            else noisy[noisyCount++] = currMove;
            if (RootNode && depth >= LOGROOTMOVEDEPTH) std::cout << "info depth " << std::dec << (int)currSearch << " currmove " << getMoveString(currMove) << " currmovenumber " << moveSearched + 1 << " currmovescore " << currMoveScore << " hashfull " << hashfull() << std::endl; // << " kCoffs: " << kCoffs << "/" << kEncounters << std::endl;

            if (moveSearched > PVNode * 3 && depth >= 3 && isQuiet)
            {
                Depth R = reduction(depth, moveSearched, PVNode, improving);
                // R -= givesCheck;
                // currMoveScore = getScore(moveList.moves[i]) - QUIETSCORE;
                if (cutNode) R += 2;
                // if (PVNode) R -= 1 + cutNode;
                // R -= std::clamp(currMoveScore / 8192, -1, 1);
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
                        updateHH(pos.side, depth, currMove, quiets, quietsCount, noisy, noisyCount);
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
    if (moveSearched == 0)
        //return excludedMove ? -infinity : (inCheck ? matedIn(ply) : randomizedDrawScore(nodes)); // Randomize draw score so that we try to explore different lines
        return inCheck ? matedIn(ply) : randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

    if (!stopped){ // && !excludedMove){
        U8 ttStoreFlag = hashNONE; // hashPVMove * ttPv;
        if (bestScore >= beta)
            ttStoreFlag |= hashLOWER;
        else
            ttStoreFlag |= hashUPPER + (alpha != origAlpha) * hashLOWER;
        //writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply, ttPv);
        writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply, false);
    }

    return bestScore;
}

Score Game::quiescence(Score alpha, Score beta)
{
    bool inCheck = pos.inCheck();
    if (inCheck)
        if (isRepetition())
            return randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

    Score bestScore;

    if (ply >= maxPly - 1)
        return bestScore;

    bestScore = inCheck ? noScore : evaluate();

    if (bestScore >= beta)
        return beta;
    alpha = std::max(alpha, bestScore);

    // Generate moves
    MoveList moveList;

    inCheck ? generateMoves(moveList, noMove, noMove, noMove) : generateCaptures(moveList);

    UndoInfo undoer = UndoInfo(pos);
    U16 moveCount = 0;
    for (int i = 0; i < moveList.count; i++)
    {
        Move move = onlyMove(moveList.moves[i]);
        // SEE pruning : skip all moves that have see < -100 (We may want to do this with a threshold of 0, but we would introduce another see call. TODO: lazily evaluate the see so that we can skip moves with see < 0)
        if (!inCheck && moveCount && getScore(moveList.moves[i]) < GOODNOISYMOVE)
            break;
        if (makeMove(move))
        {
            moveCount++;
            Score score = -quiescence(-beta, -alpha);
            undo(undoer, move);
            if (score > alpha)
            {
                alpha = score;
                if (alpha >= beta)
                    return beta;
            }
        }
        else
            undo(undoer, move);
    }

    return alpha;
}

#define ASPIRATIONWINDOW 25
void Game::startSearch(bool halveTT = true)
{

    SStack stack[4 + maxPly + 1]; // 4 is the max ply that we look back, 1 is the max ply that we look forward
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

    // killerCutoffs = 0;
    // posKillerSearches = 0;

    startTime = getTime64();

    switch (searchMode)
    {
    case 0:                 // Infinite search
        depth = maxPly - 1; // Avoid overflow
        moveTime = 0xffffffffffffffffULL;
        break;
    case 1: // Fixed depth
        moveTime = 0xffffffffffffffffULL;
        break;
#define UCILATENCYMS 10
    case 2: // Time control
        moveTime = getTime64();
        if (pos.side == WHITE)
            moveTime += (U64)(wtime / 20) + winc / 2 - UCILATENCYMS;
        else
            moveTime += (U64)(btime / 20) + binc / 2 - UCILATENCYMS;
        depth = maxPly - 1;
        break;
    case 3:
        depth = maxPly - 1;
        break;
    }

    // Clear history, captHistory and counter move tables
    // memset(historyTable, 0, sizeof(historyTable));
    // memset(counterMoveTable, 0, sizeof(counterMoveTable));
    // memset(captureHistoryTable, 0, sizeof(captureHistoryTable));

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
                std::cout << " nps " << ((nodes - locNodes) / (timer2 - timer1 + 1)) * 1000 << std::endl;
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
                std::cout << " nps " << ((nodes - locNodes) / (timer2 - timer1 + 1)) * 1000 << std::endl;
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
                std::cout << " nps " << ((nodes - locNodes) / (timer2 - timer1 + 1)) * 1000 << std::endl;
                break;
            }
        }
    }

bmove:
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