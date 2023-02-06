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

static inline Score mateIn(U16 ply) { return ((mateScore) - (ply)); }
static inline Score matedIn(U16 ply) { return ((-mateScore) + (ply)); }

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

    // Draws - mate distance
    if (!RootNode) {
#if DETECTREPETITION
        if (isRepetition()) return 0; // contempt()
#endif
        if (pos.fiftyMove >= 100) return 0; // contempt()

        alpha = std::max(alpha, matedIn(ply));
        beta = std::min(beta, mateIn(ply + 1));
        if (alpha >= beta) return alpha;
    }
    
    
#if ENABLETTSCORING
    //// TT probe
    ttEntry* tte = probeTT(pos.hashKey);
    Score ttScore = -infinity;
    Move ttMove = 0;
    U8 ttFlags;

    if (tte != nullptr && !RootNode && tte->depth >= depth) {
       ttScore = tte->score;
       ttFlags = tte->flags;
       ttMove = tte->bestMove;
       
       // Account for mate values
       ttScore += ply * (ttScore < -mateValue);
       ttScore -= ply * (ttScore > mateValue);
       
       if (ttFlags & hashEXACT) return ttScore;
       if ((ttFlags & hashALPHA)) alpha = std::max(alpha, ttScore);
       else if ((ttFlags & hashBETA)) beta = std::min(beta, ttScore);
       if (alpha >= beta) return ttScore;
    }
#endif

    // Quiescence drop
#if ENABLEQUIESCENCESEARCH
    if (depth <= 0) return quiescence(alpha, beta);
#else
    if (depth <= 0) return evaluate();
#endif

    seldepth = std::max(seldepth, (U16)(ply + 1));
    bool inCheck = pos.inCheck();
    Position save = pos;

#if ENABLENMP
    // Null move pruning
    if (!RootNode && depth >= 3 && !inCheck && okToReduce(pos.lastMove) && abs(alpha) < mateValue && abs(beta) < mateValue) {
       // make null move
       makeNullMove();
       constexpr Depth R = 2;
       Score nullScore = -search(-beta, -beta + 1, depth - R);
       restore(save);
       if (nullScore >= beta) {
           if (nullScore >= mateValue) nullScore = beta;
           return nullScore;
       }
    }
#endif

    // Search
    Score origAlpha = alpha;
    Score bestScore = -infinity;
    Move bestMove = 0;
    U16 moveSearched = 0;
    
    pvLen[ply] = ply;

    MoveList moveList;
    generateMoves(moveList); // Already sorted, except for ttMove

    //// Sort ttMove
#if ENABLETTORDERING
    if (ttMove) {
        for (int i = 0; i < moveList.count; i++) {
            if (packedMoveCompare(ttMove, (Move)moveList.moves[i])) {
                moveList.moves[0] = moveList.moves[i];
                moveList.moves[i] = 0;
                break;
            }
        }
    }
#endif

    // Iterate through moves
    for (int i = 0; i < moveList.count; i++) {
        Move currMove = onlyMove(moveList.moves[i]);
        S32 currMoveScore = getScore(moveList.moves[i]) - 16384;
        if (makeMove(currMove)) {
            ++moveSearched;
            ++nodes;
            if (RootNode && depth >= LOGROOTMOVEDEPTH) {
                std::cout << "info depth " << std::dec << (int)currSearch << " currmove " << getMoveString(currMove) << " currmovenumber " << moveSearched << " currmovescore " << currMoveScore << " hashfull " << hashfull() << std::endl;
            }
            Score score = -search(-beta, -alpha, depth - 1);
            restore(save);

            if (score > bestScore) {
                bestScore = score;
                bestMove = currMove;
                pvTable[ply][ply] = currMove;
                if (ply + 1 < maxPly) {
                    memcpy(&(pvTable[ply][ply + 1]), &(pvTable[ply + 1][ply + 1]), sizeof(Move)* (static_cast<unsigned long long>(pvLen[ply + 1]) - ply - 1));
                    pvLen[ply] = pvLen[ply + 1];
                }
            }

            if (score > alpha) {
                alpha = score;
                if (score >= beta) {
#if ENABLEHISTORYHEURISTIC
                    updateHistoryBonus(&historyTable[pos.side][moveSource(currMove)][moveTarget(currMove)], depth, true);
#endif
                    break;
                }
            }
        }
        else restore(save);
    }

    //// Check for checkmate / stalemate
    if (moveSearched == 0) return inCheck ? matedIn(ply) : 0;

#if ENABLETTSCORING
    U8 ttStoreFlag = hashINVALID;
    if (bestScore >= beta) ttStoreFlag = hashBETA;
    else {
        if (alpha != origAlpha) ttStoreFlag = hashEXACT;
        else ttStoreFlag = hashALPHA;
    }
    if (!stopped) writeTT(pos.hashKey, bestScore, -infinity, depth, ttStoreFlag, bestMove, ply);
#endif
    
    return bestScore;

}

Score Game::quiescence(Score alpha, Score beta){

    bool inCheck = pos.inCheck();

    Score standPat = evaluate();

    if (ply >= maxPly - 1) return standPat;

#if EVASIONSINQUIESCENCE
    if (!inCheck) {
        if (standPat >= beta) return beta;
#if DODELTAPRUNING
        Score bigDelta = (egValues[R] + egValues[N]) * isPromotion(pos.lastMove) + egValues[Q];
        if (standPat < alpha - bigDelta) return alpha;
#endif
        if (alpha < standPat) alpha = standPat;
    }
#else
    if (standPat >= beta) return beta;
#if DODELTAPRUNING
    Score bigDelta = (egValues[R] + egValues[N]) * isPromotion(pos.lastMove) + egValues[Q];
    if (standPat < alpha - bigDelta) return alpha;
#endif
    if (alpha < standPat) alpha = standPat;
#endif

    // Generate moves
    MoveList moveList;
#if EVASIONSINQUIESCENCE
    inCheck ? generateMoves(moveList) : generateCaptures(moveList);
#else
    generateCaptures(moveList);
#endif

#if DETECTREPETITION
    if (inCheck) if (isRepetition()) return 0;
#endif

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
    //std::cout << std::endl;

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
#if ENABLEHISTORYHEURISTIC
    memset(historyTable, 0, sizeof(historyTable));
    // memset(pieceFromHistoryTable, 0, sizeof(pieceFromHistoryTable));
    // memset(pieceToHistoryTable, 0, sizeof(pieceToHistoryTable));
#endif

#if ENABLEKILLERHEURISTIC
    memset(killerTable, 0, sizeof(killerTable));
#endif

#if ENABLECOUNTERMOVEHEURISTIC
    memset(counterMoveTable, 0, sizeof(counterMoveTable));
#endif

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
    bestMove = pvTable[0][0];
    
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
        if (stopped)
            goto bmove;
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

#if ENABLETTSCORING
    if (halveTT) {
        // Age pv table
        for (U64 i = 0; i < ttBucketCount; i++) for (U64 j = 0; j < ttBucketSize; j++) {
            tt[i].entries[j].depth = std::max(0, tt[i].entries[j].depth - 2 - 12 * (((tt[i].entries[j].flags) & hashOLD)> 0));
            tt[i].entries[j].flags |= hashOLD;
        }
    }
#endif
}