#include "Game.h"
#include "tables.h"
#include "constants.h"
#include "history.h"
#include "zobrist.h"
#include "evaluation.h"
#include "uci.h"
#include "tt.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
// Global var to debug the SE implementation
S64 seCandidates = 0;
U64 seActivations = 0;
S64 avgDist = 0;


static inline Score mateIn(Ply ply) { return ((mateScore) - (ply)); }
static inline Score matedIn(Ply ply) { return ((-mateScore) + (ply)); }
static inline Score randomizedDrawScore(U64 nodes) { return 4 - (nodes & 7); }
static inline Score adjustMateScore(Score score, Ply ply) { return score + ply * (score < -mateValue) - ply * (score > mateValue); }

inline bool okToReducePacked(Position &pos, PackedMove move)
{
    return !((movePiece(move) != NOPIECE) | (pos.pieceOn(moveTarget(move)) != NOPIECE));
}

static inline S32 reduction(Depth d, U16 m, bool isQuiet, bool isPv)
{
    return reductionTable[isQuiet][std::min((int)d,64)][std::min((int)m,64)] - lmrPV() * isPv;
}

static inline Score futilityMargin(Depth depth, bool improving)
{
    return futilityMarginDelta() * (depth - improving);
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

    if ((nodes & comfrequency) == 0) {
        communicate();
        if (stopped)
            return 0;
    }

    //print();
    //std::cout << "lmove ";
    //printMove((ss-1)->move);
    //std::cout << " " << std::endl;

    assert(pos.hashKey == pos.generateHashKey());
    assert(pos.pawnHashKey == pos.generatePawnHashKey());
    assert(pos.nonPawnKeys[WHITE] == pos.generateNonPawnHashKey(WHITE));
    assert(pos.nonPawnKeys[BLACK] == pos.generateNonPawnHashKey(BLACK));
    assert(pos.minorKey == pos.generateMinorHashKey());
    assert(pos.majorKey == pos.generateMajorHashKey());
    // Ply overflow
    if (ply >= maxPly)
        return evaluate();
    const bool RootNode = ply == 0;
    const bool PVNode = (beta - alpha) > 1;

    Score eval;
    Score rawEval; // for corrhist
    Score improvement;
    bool improving = true;
    const Move excludedMove = ss->excludedMove;
    // Guard from pvlen editing when in singular extension
    if (!excludedMove) 
        pvLen[ply] = ply;

    // Update seldepth
    seldepth = std::max(Ply(seldepth), Ply(ply + 1));

    const bool inCheck = pos.checkers;

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
    ttEntry *tte = excludedMove ? nullptr : probeTT(pos.hashKey);
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

    // Quiescence drop
    if (depth <= 0) return quiescence(alpha, beta, ss);

    (ss)->doubleExtensions = (ss-1)->doubleExtensions;

    // else if (depth < ttDepth && (ttBound & hashLOWER)){
    //     if (ply == 1) depth += std::min(3, ttDepth - depth);
    //     else depth++; // Principled iterative extensions: If the tt entry is a lower bound (or exact), we don't search shallower than the tt depth
    // }
    // Initialize the undoer
    UndoInfo undoer = UndoInfo(pos);

    if (!excludedMove){
        if (PVNode && depth >= IIRDepth() && !ttMove) --depth; 
        if (cutNode && depth >= IIRDepth() && !ttMove) --depth; 
    }

    if (inCheck)
    {
        ss->staticEval = eval = rawEval = noScore;
        improvement = 0;
        improving = false;
        goto skipPruning;
    }
    
    // Get static eval of the position
    if (ttHit)
    {
        // Check if the eval is stored in the TT
        rawEval = tte->eval != noScore ? tte->eval : evaluate();
        eval = ss->staticEval = ttMove ? rawEval : correctStaticEval(pos, rawEval);
        // Also, we might be able to use the score as a better eval
        if (ttScore != noScore && (ttBound == hashEXACT || (ttBound == hashUPPER && ttScore < eval) || (ttBound == hashLOWER && ttScore > eval)))
            eval = ttScore;
    }
    else if (excludedMove){
        rawEval = eval = ss->staticEval; // We already have the eval from the main search in the current ss entry
        improvement = 0;
        improving = false;
        goto skipPruning;
    }
    else {
        rawEval = evaluate();
        eval = ss->staticEval = correctStaticEval(pos, rawEval);
        // Store the eval in the TT if not in exclusion mode (in which we might already have the entry)
        writeTT(pos.hashKey, noScore, eval, 0, hashNONE, 0, ply, PVNode, ttPv);
    }

    improvement = [&](){
        if ((ss - 2)->staticEval != noScore)
            return ss->staticEval - (ss - 2)->staticEval;
        else if ((ss - 4)->staticEval != noScore)
            return ss->staticEval - (ss - 4)->staticEval;
        return 1; // speculative improvement to cut less
    }();

    // Calculate the improving flag
    improving = improvement > 0;

    // Pruning time
    if (!PVNode && !excludedMove)
    {
        // RFP
        if (depth <= RFPDepth() && abs(eval) < mateValue && eval - futilityMargin(depth, improving) >= beta) // && !excludedMove)
            return eval;

        // Null move pruning
        if (eval >= ss->staticEval &&
            eval >= beta &&
            (ss - 1)->move &&
            depth >= 3 &&
            okToReduce((ss - 1)->move) && pos.hasNonPawns() &&
            ply >= nmpPlies)
        {

            // make null move
            makeNullMove();
            prefetch(&tt[hashEntryFor(pos.hashKey)]);
            ss->move = noMove;
            ss->contHistEntry = continuationHistoryTable[0];

            Depth R = nmpQ1() + (depth / nmpDepthDivisor()) + std::min((eval - beta) / nmpScoreDivisor(), nmpQ2());
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
        if (depth <= razorDepth() && abs(eval) < mateValue && eval + razorQ1() + depth * razorQ2() < alpha)
        {
            const Score razorScore = quiescence(alpha, beta, ss);
            if (razorScore <= alpha) return razorScore;
        }
    
    }

skipPruning:
    

    // Search
    const Score origAlpha = alpha;
    Score bestScore = noScore;
    Move bestMove = noMove;
    U16 moveSearched = 0;
    bool skipQuiets = false;

    Move quiets[256], noisy[256];
    U16 quietsCount = 0, noisyCount = 0;

    MoveList moveList;
    generateMoves(moveList, ss);
    // Iterate through moves
    for (int i = (ttMove ? sortTTUp(moveList, ttMove) : 1); i < moveList.count; i++) // Slot 0 is reserved for the tt move, wheter it is present or not
    {
        S32 currMoveScore = getScore(moveList.moves[i]); // - BADNOISYMOVE;
        Move currMove = onlyMove(moveList.moves[i]);
        if (sameMovePos(currMove, excludedMove)) continue;
        const bool isQuiet = okToReduce(currMove);
        if (!skipQuiets) { 
            if (!PVNode && moveSearched >= lmpMargin[depth][improving]) skipQuiets = true;
            if (!PVNode
                && depth <= 8
                && !inCheck
                && bestScore > -KNOWNWIN
                && std::abs(alpha) < KNOWNWIN
                && isQuiet
                && ss->staticEval + futPruningAdd() + futPruningMultiplier() * depth <= alpha)
            {
                skipQuiets = true;
                continue;
            }
            if (!PVNode && depth <= 4 && (isQuiet ? (currMoveScore - QUIETSCORE) : (currMoveScore - BADNOISYMOVE)) < ( historyPruningMultiplier() * depth) + historyPruningBias()){
                skipQuiets = true;
                continue;
            }
        }
        else if (currMoveScore < COUNTERSCORE) continue;
        // assert (
        //     i != 0 || !excludedMove ||
        //     (excludedMove == currMove)
        // );
        if (!currMove) continue; // || currMove == excludedMove

        // const bool givesCheck = isCheck(currMove) || pos.inCheck();
        
        if (makeMove(currMove))
        {
            // Prefetch tt
            prefetch(&tt[hashEntryFor(pos.hashKey)]);
            U64 nodesBefore = nodes;
            // // Singular extension
            Depth extension = 0;
            if (!excludedMove && ply < currSearch * (1 + PVNode)){
                
                if (i == 0 // Can only happen on ttMove
                    && !RootNode 
                    && depth >= singularSearchDepth()
                    && (ttBound & hashLOWER) 
                    && abs(ttScore) < mateValue 
                    && ttDepth >= depth - 3
                ){
                    // Increase singular candidates
                    //++seCandidates;
                    const Score singularBeta = ttScore - singularDepthMultiplier() * depth / 10;
                    const Depth singularDepth = (depth - 1) / 2;
                    ss->excludedMove = currMove;
                    undo(undoer, currMove);
                    const Score singularScore = search(singularBeta - 1, singularBeta, singularDepth, cutNode, ss);
                    makeMove(currMove);
                    ss->excludedMove = noMove;

                    if (singularScore < singularBeta) {
                        extension = 1;
                        if (!PVNode && ss->doubleExtensions < maximumDoubleExtensions() && singularScore + doubleExtensionMargin() < singularBeta) {
                            ++ss->doubleExtensions;
                            //std::cout << "Double extension counter "<<ss->doubleExtensions<<std::endl;
                            extension = 2;
                        }
                        // Increase singular activations
                        // ++seActivations;
                    }
                    else if (singularBeta >= beta){ // Multicut
                        undo(undoer, currMove);
                        return singularBeta;
                    }
                    else if (ttScore >= beta){
                        extension = -1;
                    }
                    
                    // else{
                    //     std::cout << "info string Singular failed with score: " << singularScore << " beta: " << singularBeta << std::endl;
                    // }
                    // Update avg dist
                    //avgDist += singularScore - singularBeta;
                }
                else if (inCheck)
                    extension = 1;
            }

            Depth newDepth = std::max(0,depth - 1 + extension);

            ss->move = currMove;
            ss->contHistEntry = continuationHistoryTable[indexPieceTo(movePiece(currMove), moveTarget(currMove))];
            
            Score score;
        
            ++nodes;
            if (isQuiet) quiets[quietsCount++] = currMove;
            else noisy[noisyCount++] = currMove;
            // if (RootNode && depth >= LOGROOTMOVEDEPTH) std::cout << "info depth " << std::dec << (int)currSearch << " currmove " << getMoveString(currMove) << " currmovenumber " << moveSearched + 1 << " currmovescore " << currMoveScore << " hashfull " << hashfull() << std::endl; // << " kCoffs: " << kCoffs << "/" << kEncounters << std::endl;

            if (moveSearched > PVNode * 3 && depth >= 3 && (isQuiet || !ttPv))
            {
                S32 granularR = reduction(depth, moveSearched, isQuiet, ttPv);
                if (currMoveScore >= COUNTERSCORE) granularR -= lmrExpectedDecent();
                if (isQuiet){
                    // R -= givesCheck;
                    granularR -= std::clamp((currMoveScore - QUIETSCORE) * RESOLUTION, -16000000LL, 16000000LL) / lmrQuietHistoryDivisor();
                    if (cutNode) granularR += lmrQuietCutNode();
                    if (ttPv) granularR -= cutNode * lmrQuietTTPV();
                }
                else {
                    if (currMoveScore < QUIETSCORE) { 
                        if (cutNode) granularR += lmrBadNoisyCutNode();
                        granularR -= std::clamp((currMoveScore - BADNOISYMOVE) * RESOLUTION, -6000000LL, 12000000LL) / lmrNoisyHistoryDivisorA();
                                        }
granularR -= std::clamp((currMoveScore - GOODNOISYMOVE - BADNOISYMOVE) * RESOLUTION, -6000000LL, 12000000LL) / lmrNoisyHistoryDivisorB();
                }
                // The function looked cool on desmos
                granularR -= lmrCieckA() * improvement / (std::abs(improvement * lmrCieckB() / 1000) + lmrCieckC());
                Depth R = granularR / RESOLUTION;
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
        return excludedMove ? alpha : (inCheck ? matedIn(ply) : randomizedDrawScore(nodes)); // Randomize draw score so that we try to explore different lines
        // return inCheck ? matedIn(ply) : randomizedDrawScore(nodes);
    }
        // 
        // return inCheck ? matedIn(ply) : randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

    if (!stopped && !excludedMove){
        if (!inCheck
            && (!bestMove || okToReduce(bestMove))
            && !(ttBound == hashLOWER && bestScore <= ss->staticEval)
            && !(ttBound == hashUPPER && bestScore >= ss->staticEval)){
                updateCorrHist(pos, bestScore - ss->staticEval, depth);
        }
        U8 ttStoreFlag = bestScore >= beta ? hashLOWER : alpha != origAlpha ? hashEXACT : hashUPPER;
        writeTT(pos.hashKey, bestScore, rawEval, depth, ttStoreFlag, bestMove, ply, PVNode, ttPv);
        // writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply, false, PVNode);
    }

    return bestScore;
}

Score Game::quiescence(Score alpha, Score beta, SStack *ss)
{
    const bool inCheck = pos.checkers;
    if (inCheck)
        if (isRepetition())
            return randomizedDrawScore(nodes); // Randomize draw score so that we try to explore different lines

    Score bestScore;
    Score rawEval;

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
        ss->staticEval = bestScore = rawEval = noScore;
    }
    else if (ttHit){
        rawEval = tte->eval != noScore ? tte->eval : evaluate();
        ss->staticEval = bestScore = ttMove ? rawEval : correctStaticEval(pos, rawEval);
        if (ttScore != noScore && (ttBound == hashEXACT || (ttBound == hashUPPER && ttScore < bestScore) || (ttBound == hashLOWER && ttScore > bestScore)))
            bestScore = ttScore;
    }
    else {
        rawEval = evaluate();
        ss->staticEval = bestScore = correctStaticEval(pos, rawEval);
        writeTT(pos.hashKey, noScore, bestScore, 0, hashNONE, 0, ply, PVNode, ttPv);
    }

    if (bestScore >= beta)
        return bestScore;
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
            // Prefetch tt
            prefetch(&tt[hashEntryFor(pos.hashKey)]);
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
    writeTT(pos.hashKey, bestScore, rawEval, 0, ttStoreFlag, bestMove, ply, PVNode, ttPv);

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

    lastScore = 0;

    startTime = getTime64();
    U64 optim = 0xffffffffffffffffULL;
#define UCILATENCYMS 20
    switch (searchMode) {
        case 0:               // Infinite search
            depth = maxPly - 1; // Avoid overflow
        case 1: // Fixed depth
            optim = moveTime = 0xffffffffffffffffULL;
            break;
        case 2: // Time control
            moveTime = getTime64();
            U64 totalTime;
            if (pos.side == WHITE) totalTime = (U64)(wtime * timeTmA() / RESOLUTION) + winc * timeTmB() / RESOLUTION;
            else totalTime = (U64)(btime * timeTmA() / RESOLUTION) + binc * timeTmB() / RESOLUTION;
            totalTime -= UCILATENCYMS;
            optim = totalTime * timeTmOptimScale() / 1000;

            moveTime += totalTime;
            depth = maxPly - 1;
            break;
        case 3:
            moveTime -= UCILATENCYMS;
            optim = moveTime;
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

    std::cout << "info depth 1 score cp " << score << " nodes " << nodes << " moves ";
    printMove(bestMove);
    std::cout << std::endl;
    
    depth = std::min(depth, Depth(maxPly - 3));
    if (depth < 0)
        depth = maxPly - 3;
    if (stopped)
        goto bmove;

    for (Depth searchDepth = 2; (searchDepth <= depth) && searchDepth >= 2 && !stopped; searchDepth++)
    {
        currSearch = searchDepth;
        delta = ASPIRATIONWINDOW;
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
                    std::cout << std::dec << "info depth " << (int)searchDepth << " score mate " << -(mateScore + score + 2) / 2 << " lowerbound nodes " << nodes << " pv ";
                else if (score > mateValue && score < mateScore)
                    std::cout << std::dec << "info depth " << (int)searchDepth << " score mate " << (mateScore + 1 - score) / 2 << " lowerbound nodes " << nodes << " pv ";
                else
                    std::cout << std::dec << "info depth " << (int)searchDepth << " score cp " << (score >> 1) << " lowerbound nodes " << nodes << " pv ";
                printMove(pvTable[0][0]);
                std::cout << " nps " << (((nodes - locNodes) * 1000) / (timer2 - timer1 + 1)) << std::endl;
                delta *= 1.44;
                currSearch = searchDepth;
            }
            else if (score >= beta)
            {
                beta = std::min(S32(infinity), score + delta);
                if (score < -mateValue && score > -mateScore)
                    std::cout << std::dec << "info depth " << (int)searchDepth << " score mate " << -(mateScore + score + 2) / 2 << " upperbound nodes " << nodes << " pv ";
                else if (score > mateValue && score < mateScore)
                    std::cout << std::dec << "info depth " << (int)searchDepth << " score mate " << (mateScore + 1 - score) / 2 << " upperbound nodes " << nodes << " pv ";
                else
                    std::cout << std::dec << "info depth " << (int)searchDepth << " score cp " << (score >> 1) << " upperbound nodes " << nodes << " pv ";
                printMove(pvTable[0][0]);
                std::cout << " nps " << (((nodes - locNodes) * 1000) / (timer2 - timer1 + 1)) << std::endl;
                delta *= 1.44;
                currSearch = std::max(4,std::max(searchDepth - 5, currSearch - 1));
            }
            else
            {
                if (score < -mateValue && score > -mateScore)
                    std::cout << std::dec << "info score mate " << -(mateScore + score + 2) / 2 << " depth " << (int)searchDepth << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";
                else if (score > mateValue && score < mateScore)
                    std::cout << std::dec << "info score mate " << (mateScore + 1 - score) / 2 << " depth " << (int)searchDepth << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";
                else
                    std::cout << std::dec << "info score cp " << (score >> 1) << " depth " << (int)searchDepth << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";

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
            // 1.242 felt cute, maybe it gains
            nodesTmScale = ((S64)nodesTmMax() - ((double)nodesPerMoveTable[indexFromTo(moveSource(bestMove), moveTarget(bestMove))] / (double)nodes) * (S64)nodesTmMul()) / RESOLUTION;    
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
        for (U64 i = 0; i < ttEntryCount; i++){
            tt[i].flags |= hashOLD;
        }
    }
}
