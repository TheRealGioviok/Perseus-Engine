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

static inline Depth reduction(Depth d, U16 m, bool isPv){
    S32 r = reductionTable[d] * reductionTable[m];
    return std::max(0, ((r + 1642 - (512 * isPv)) / 1024));
}

static inline void sortTTUp(MoveList& ml, Move ttMove){
    for (int i = 0; i < ml.count; i++) {
        if (packedMoveCompare(ttMove, (Move)ml.moves[i])) {
            ml.moves[0] = ml.moves[i];
            ml.moves[i] = 0;
            break;
        }
    }
}

static inline void updateKillers(Move killer, Ply ply){
    if (killer != killerTable[0][ply]){
        killerTable[1][ply] = killerTable[0][ply];
        killerTable[0][ply] = killer;
    }
}

static inline void updateCounters(Move counter, Move lastMove){
    if (isOk(lastMove)) counterMoveTable[movePiece(lastMove)][moveTarget(lastMove)] = counter;
}

static inline void clearNextPlyKillers(Ply ply){
    if (ply < maxPly - 1){
        killerTable[0][ply + 1] = 0;
        killerTable[1][ply + 1] = 0;
    }
}

Score Game::search(Score alpha, Score beta, Depth depth) {
    // Comms
    if (stopped) return 0;
    if ((nodes & comfrequency) == 0) {
        communicate();
        if (stopped) return 0;
    }
    assert(pos.hashKey == pos.generateHashKey());
    // Ply overflow
    if (ply > maxPly) return evaluate();
    
    pvLen[ply] = ply;

    const bool RootNode = ply == 0;
    const bool PVNode = beta - alpha > 1;

    // Draws - mate distance
    if (!RootNode) {
        if (isRepetition()) return 0; // contempt()

        if (pos.fiftyMove >= 100) return 0; // contempt()

        alpha = std::max(alpha, matedIn(ply));
        beta = std::min(beta, mateIn(ply + 1));
        if (alpha >= beta) return alpha;
    }
    
    //// TT probe
    ttEntry* tte = probeTT(pos.hashKey);
    Score ttScore = -infinity;
    Move ttMove = 0;
    U8 ttFlags;
    if (tte != nullptr) {
        ttScore = tte->score;
        ttFlags = tte->flags;
        ttMove = tte->bestMove;

        // Account for mate values
        ttScore += ply * (ttScore < -mateValue);
        ttScore -= ply * (ttScore > mateValue);
        if (!RootNode && tte->depth >= depth && !PVNode){
            if (ttFlags & hashEXACT) return ttScore;
            if ((ttFlags & hashLOWER)) alpha = std::max(alpha, ttScore);
            else if ((ttFlags & hashUPPER)) beta = std::min(beta, ttScore);
            if (alpha >= beta ) return ttScore;
        }
    }

    // Quiescence drop
    if (depth <= 0) return quiescence(alpha, beta);

    clearNextPlyKillers(ply);

    seldepth = std::max(seldepth, (U16)(ply + 1));

    bool inCheck = pos.inCheck();
    Position save = pos;

    // Null move pruning
    if (!PVNode && depth >= 3 && !inCheck && isOk(pos.lastMove) && okToReduce(pos.lastMove) && !pos.mayBeZugzwang() && abs(alpha) < mateValue && abs(beta) < mateValue) {
       // make null move
       makeNullMove();
       Depth R = 2;
       Score nullScore = -search(-beta, -beta + 1, depth - R);
       restore(save);
       if (nullScore >= beta) {
           if (nullScore >= mateValue) nullScore = beta;
           return nullScore;
       }
    }

    // Search
    Score origAlpha = alpha;
    Score bestScore = -infinity;
    Move bestMove = 0;
    U16 moveSearched = 0;
    
    pvLen[ply] = ply;

    MoveList moveList;
    Move quiets[128];
    U16 quietsCount = 0;
    generateMoves(moveList, ply); // Already sorted, except for ttMove

    //// Sort ttMove
    if (ttMove) sortTTUp(moveList, ttMove);

    // Iterate through moves
    for (int i = 0; i < moveList.count; i++) {
        Move currMove = onlyMove(moveList.moves[i]);
        S32 currMoveScore = getScore(moveList.moves[i]) - 16384;
        if (makeMove(currMove)) {
            Score score;

            ++moveSearched;
            ++nodes;

            if (okToReduce(currMove)) quiets[quietsCount++] = currMove;
            if (RootNode && depth >= LOGROOTMOVEDEPTH) std::cout << "info depth " << std::dec << (int)currSearch << " currmove " << getMoveString(currMove) << " currmovenumber " << moveSearched << " currmovescore " << currMoveScore << " hashfull " << hashfull() << std::endl;
            
            if (moveSearched == 0) score = -search(-beta, -alpha, depth - 1);
            else {
                score = -search(-alpha - 1, -alpha, depth - 1);
                if (PVNode && (score > alpha && score < beta)) score = -search(-beta, -alpha, depth - 1);
            }
            restore(save);

            if (score > bestScore) {
                bestScore = score;
                bestMove = currMove;
                if (PVNode){
                    pvTable[ply][ply] = currMove;
                    if (ply + 1 < maxPly) {
                        memcpy(&(pvTable[ply][ply + 1]), &(pvTable[ply + 1][ply + 1]), sizeof(Move)* (static_cast<unsigned long long>(pvLen[ply + 1]) - ply - 1));
                        pvLen[ply] = pvLen[ply + 1];
                    }
                }
            }

            if (score > alpha) {
                alpha = score;
                if (score >= beta) {
                    if (okToReduce(currMove)){
                        updateKillers(currMove, ply);
                        updateCounters(currMove, pos.lastMove);
                    }
                    updateHH(pos.side, depth, currMove, quiets, quietsCount);
                    break;
                }
            }
        }
        else restore(save);
    }

    //// Check for checkmate / stalemate
    if (moveSearched == 0) return inCheck ? matedIn(ply) : 0;

    U8 ttStoreFlag = hashINVALID;
    if (bestScore >= beta) ttStoreFlag = hashLOWER;
    else {
        if (alpha != origAlpha) ttStoreFlag = hashEXACT;
        else ttStoreFlag = hashUPPER;
    }
    if (!stopped) writeTT(pos.hashKey, bestScore, -infinity, depth, ttStoreFlag, bestMove, ply);
    
    return bestScore;

}

Score Game::quiescence(Score alpha, Score beta){

    bool inCheck = pos.inCheck();
    if (inCheck) if (isRepetition()) return 0;

    Score standPat = evaluate();

    if (ply >= maxPly - 1) return standPat;

    if (!inCheck) {
        if (standPat >= beta) return beta;
        Score bigDelta = (egValues[R] + egValues[N]) * isPromotion(pos.lastMove) + egValues[Q];
        if (standPat < alpha - bigDelta) return alpha;
        if (alpha < standPat) alpha = standPat;
    }
    
    // Generate moves
    MoveList moveList;

    inCheck ? generateMoves(moveList, ply) : generateCaptures(moveList, ply);

    Position save = pos;

    for (int i = 0; i < moveList.count; i++){
        Move move = onlyMove(moveList.moves[i]);
        if (makeMove(move)) {
            Score score = -quiescence(-beta, -alpha);
            restore(save);
            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) return beta;
            }
        }
        else restore(save);
    }

    return alpha;
}

void Game::startSearch(bool halveTT = true){
    nodes = 0ULL;
    stopped = false;
    ply = 0;
    seldepth = 0;

    std::cin.clear();

    pos.lastMove = 0;
    lastScore = 0;

    startTime = getTime64();

    switch (searchMode){
        case 0: // Infinite search
            depth = maxPly-1; // Avoid overflow
            moveTime = 0xffffffffffffffffULL;
            break;
        case 1: // Fixed depth
            moveTime = 0xffffffffffffffffULL;
            break;
#define UCILATENCYMS 10
        case 2: // Time control
             moveTime = getTime64();
            if (pos.side == WHITE) moveTime += (U64)(wtime / 20) + winc/2 - UCILATENCYMS;
            else moveTime += (U64)(btime / 20) + binc/2 - UCILATENCYMS;
            depth = maxPly - 1;
            break;
        case 3:
            depth = maxPly - 1;
            break;
    }
	
    // Clear history, killer and counter move tables
    memset(historyTable, 0, sizeof(historyTable));
    memset(killerTable, 0, sizeof(killerTable));
    memset(counterMoveTable, 0, sizeof(counterMoveTable));

    // Clear pv len and pv table
    memset(pvLen, 0, sizeof(pvLen));
    memset(pvTable, 0, sizeof(pvTable));
    
#if USINGEVALCACHE
    // Clear evaluation hash table
    for (U64 i = 0; i < (evalHashSize); i++) evalHash[i].score = -infinity;
#endif

    // Always compute a depth 1 search
    rootDelta = 2 * infinity;
    currSearch = 1;

    Score score = search(-infinity, infinity, 1);
    Move bestMove = pvTable[0][0];
    
    std::cout << "info score depth 1 cp " << score << " nodes " << nodes << " moves " ;
    printMove(bestMove);
    std::cout << std::endl;

    if (depth < 0) depth = maxPly - 1;
    if (stopped) goto bmove;
    
    for (currSearch = 2; (currSearch <= depth) && currSearch >= 2 && !stopped; currSearch++) {
        seldepth = 0; // Reset seldepth
        ply = 0;
        S64 locNodes = nodes; // Save nodes for nps calculation
        U64 timer1 = getTime64(); // Save time for nps calculation
        score = search(-infinity, infinity, currSearch); // Search at depth currSearch
        if (stopped) goto bmove;
        bestMove = pvTable[0][0];
        U64 timer2 = getTime64();
        if (score < -mateValue && score > -mateScore)
            std::cout << std::dec << "info score mate " << -(mateScore + score + 2) / 2 << " depth " << (int)currSearch << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";
        else if (score > mateValue && score < mateScore)
            std::cout << std::dec << "info score mate " << (mateScore + 1 - score) / 2 << " depth " << (int)currSearch << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";
        else
            std::cout << std::dec << "info score cp " << (score >> 1) << " depth " << (int)currSearch << " seldepth " << (int)seldepth << " nodes " << nodes << " hashfull " << hashfull() << " pv ";

        for (int i = 0; i < pvLen[0]; i++){
            printMove(pvTable[0][i]);
            std::cout << " ";
        }
        std::cout << " nps " << ((nodes - locNodes) / (timer2 - timer1 + 1)) * 1000 << std::endl;
    }

bmove:
    std::cout << "bestmove ";
    printMove(bestMove);
    std::cout << std::endl;

    if (halveTT) {
        // Age pv table
        for (U64 i = 0; i < ttEntryCount; i++) {
            tt[i].depth = std::max(Depth(0), Depth(tt[i].depth - Depth(2)));
            tt[i].flags |= hashOLD;
        }
    }
}