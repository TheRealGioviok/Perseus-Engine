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

static inline void sortTTUp(MoveList &ml, PackedMove ttMove)
{
    for (int i = 0; i < ml.count; i++)
    {
        if (packedMoveCompare(ttMove, (Move)ml.moves[i]))
        {
            ml.moves[0] = ml.moves[i];
            ml.moves[i] = 0;
            break;
        }
    }
}

static inline void updateKillers(Move killerMove, Ply ply)
{
    if (killerTable[0][ply] != killerMove)
    {
        killerTable[1][ply] = killerTable[0][ply];
        killerTable[0][ply] = killerMove;
    }
}

static inline void updateCounters(Move counter, Move lastMove)
{
    if (isOk(lastMove))
        counterMoveTable[movePiece(lastMove)][moveTarget(lastMove)] = counter;
}

static inline void clearNextPlyKillers(Ply ply, Depth depth)
{
    if (ply < maxPly - depth)
    {
        killerTable[0][ply + depth] = 0;
        killerTable[1][ply + depth] = 0;
    }
}

Score Game::search(Score alpha, Score beta, Depth depth, SStack *ss)
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

    pvLen[ply] = ply;

    const bool RootNode = ply == 0;
    const bool PVNode = beta - alpha > 1;

    Score eval = -infinity;
    Move excludedMove = ss->excludedMove;
    bool improving = false;

    bool inCheck = isCheck(pos.lastMove) || pos.inCheck();
    if (inCheck && (PVNode || ply <= currSearch))
        depth = std::max(1, depth + 1);

    // Draws - mate distance
    if (!RootNode)
    {
        if (isRepetition() || pos.fiftyMove >= 100)
            return 8 - (nodes & 7); // Randomize draw score so that we try to explore different lines

        // Mate distance pruning
        alpha = std::max(alpha, matedIn(ply));
        beta = std::min(beta, mateIn(ply + 1));
        if (alpha >= beta)
            return alpha;
    }

    //// TT probe
    ttEntry *tte = excludedMove ? nullptr : probeTT(pos.hashKey);
    Score ttScore = -infinity;
    Score ttEval = -infinity;
    Depth ttDepth = 0;
    PackedMove ttMove = 0;
    U8 ttFlags;
    if (tte != nullptr)
    {
        ttScore = tte->score;
        ttFlags = tte->flags;
        ttMove = tte->bestMove;
        ttEval = tte->eval;
        ttDepth = tte->depth;

        // Account for mate values
        ttScore += ply * (ttScore < -mateValue);
        ttScore -= ply * (ttScore > mateValue);
        if (!RootNode && tte->depth >= depth && !PVNode)
        {
            if (ttFlags & hashEXACT)
                return ttScore;
            if ((ttFlags & hashLOWER))
                alpha = std::max(alpha, ttScore);
            else if ((ttFlags & hashUPPER))
                beta = std::min(beta, ttScore);
            if (alpha >= beta)
                return ttScore;
        }
    }

    // Quiescence drop
    if (depth <= 0)
        return quiescence(alpha, beta);

    // IIR by Ed Schröder
    // http://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=64085e3396554f0fba414404445b3120
    if (depth >= IIRdepth && !ttMove && isOk((ss - 1)->move) && !excludedMove && !PVNode)
        depth--;

    clearNextPlyKillers(ply, 1);

    seldepth = std::max(Ply(seldepth), Ply(ply + 1));

    UndoInfo undoer = UndoInfo(pos);

    if (inCheck || excludedMove)
    {
        ss->staticEval = -infinity;
        improving = false;
        goto skipPruning;
    }

    ss->staticEval = eval = ttScore != -infinity ? ttEval : evaluate();
    improving = (ply > 1) && (ss->staticEval > (ss - 2)->staticEval || (ss - 2)->staticEval == -infinity);

    // Pruning time
    if (!PVNode)
    {

        // If we have ttHit, we can use the score as a better eval
        if (ttScore != -infinity)
            eval = ttScore;

        // RFP
        if (depth <= RFPDepth && eval - futilityMargin(depth, improving) >= beta)
            return eval;

        // Null move pruning
        if (ss->staticEval >= beta + nmpBias &&
            eval >= beta &&
            ss->move != 0 &&
            depth >= 3 &&
            isOk(pos.lastMove) &&
            okToReduce(pos.lastMove) && !pos.mayBeZugzwang())
        {
            // make null move
            makeNullMove();
            ss->move = 0;
            Depth R = nmpQ1 + (depth / nmpDepthDivisor) + std::min((eval - beta) / nmpScoreDivisor, nmpQ2);
            Score nullScore = -search(-beta, -beta + 1, depth - R, ss + 1);
            undoNullMove(undoer);
            if (stopped)
                return 0;
            if (nullScore >= beta)
            {
                if (nullScore >= mateValue)
                    nullScore = beta;
                return nullScore;
            }
        }

        // TODO: Razoring
        if (depth <= razorDepth && eval + razorQ1 + razorQ2 * (depth - 1) <= alpha)
            return quiescence(alpha, beta);
    }

skipPruning:

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
    if (ttMove)
        sortTTUp(moveList, ttMove);
    Move unpackedTTMove = onlyMove(moveList.moves[0]);

    // Iterate through moves
    for (int i = 0; i < moveList.count; i++)
    {
        Move currMove = onlyMove(moveList.moves[i]);
        if (currMove == excludedMove)
            continue;

        Depth extension = 0;
        // TT move extension
        if (isOk(currMove) && depth >= singularDepth && !excludedMove && ply && currMove == ttMove && (ttFlags == hashLOWER || ttFlags == hashEXACT) && abs(ttScore) < mateValue && ttDepth >= depth - 3)
        {
            Score singularBeta = ttScore - singularDepthMultiplier * depth;
            Depth singularDepth = (depth - 1) / 2;

            ss->excludedMove = ttMove;
            Score singularScore = search(singularBeta - 1, singularBeta, singularDepth, ss + 1);
            ss->excludedMove = 0;

            if (singularScore < singularBeta)
                extension = 1;
            else if (singularBeta >= beta && !PVNode)
                return singularBeta;
        }

        Depth newDepth = depth + extension;

        S32 currMoveScore = getScore(moveList.moves[i]) - 16384;
        ss->move = currMove;
        if (makeMove(currMove))
        {
            Score score;
            bool givesCheck = isCheck(currMove) || pos.inCheck(); // While isCheck finds direct checks, it won't find discovered checks

            ++nodes;

            if (okToReduce(currMove))
                quiets[quietsCount++] = currMove;
            if (RootNode && depth >= LOGROOTMOVEDEPTH)
                std::cout << "info depth " << std::dec << (int)currSearch << " currmove " << getMoveString(currMove) << " currmovenumber " << moveSearched + 1 << " currmovescore " << currMoveScore << " hashfull " << hashfull() << std::endl; // << " kCoffs: " << kCoffs << "/" << kEncounters << std::endl;
            bool doFullSearch = false;
            if (moveSearched >= 5 && !inCheck && okToReduce(currMove) && depth >= 3)
            {
                Depth R = reduction(depth, moveSearched, PVNode, improving);
                // Give a bonus to moves that are likely to be good
                R -= currMoveScore > goodHistoryThreshold;
                // Reduce less if the move gives check
                R -= givesCheck;
                // If ttMove is present, reduce more the moves that are quiet
                R += (!PVNode && unpackedTTMove);
                // Help tactical moves to stand out:
                // 1: If the move is a pawn move to 6th or 7th rank, reduce less
                //R -= (movePiece(currMove) == P && rankOf(moveTarget(currMove)) >= 2) ||
                //   (movePiece(currMove) == p && rankOf(moveTarget(currMove)) <= 5) ;
                // 2: If alpha has not been raised and current move has a good history score, reduce less
                // R -= (currMoveScore > goodHistoryThreshold && alpha == origAlpha);
                // 3: If the ttMove is quiet and it failed, reduce less
                // R -= (unpackedTTMove && bestMove != unpackedTTMove && okToReduce(unpackedTTMove));
                // 4: If we are improving but still haven't raised alpha, reduce less the moves that are most likely to be good
                R -= (improving && alpha == origAlpha && currMoveScore > goodHistoryThreshold);
                // Ensure we don't reduce into quiescence
                R = std::min(R, Depth(depth - 1));
                // Ensure we are not extending
                R = std::max(R, Depth(1));
                score = -search(-alpha - 1, -alpha, newDepth - R, ss + 1);
                doFullSearch = (score > alpha) && (R != 1);
            }
            else
                doFullSearch = !PVNode || moveSearched > 0;
            if (doFullSearch)
                score = -search(-alpha - 1, -alpha, newDepth - 1, ss + 1);
            if (PVNode && (moveSearched == 0 || (score > alpha && score < beta)))
                score = -search(-beta, -alpha, newDepth - 1, ss + 1);
            undo(undoer, currMove);

            ++moveSearched;

            if (score > bestScore)
            {
                bestScore = score;
                bestMove = currMove;
                if (PVNode)
                {
                    pvTable[ply][ply] = currMove;
                    if (ply + 1 < maxPly)
                    {
                        memcpy(&(pvTable[ply][ply + 1]), &(pvTable[ply + 1][ply + 1]), sizeof(Move) * (static_cast<unsigned long long>(pvLen[ply + 1]) - ply - 1));
                        pvLen[ply] = pvLen[ply + 1];
                    }
                }
            }

            if (score > alpha)
            {
                alpha = score;
                if (score >= beta)
                {
                    if (okToReduce(currMove))
                    {
                        updateKillers(currMove, ply);
                        Move lastMove = pos.lastMove;
                        // Move lastLastMove = ply > 1 ? (ss - 2)->move : 0;
                        updateCounters(currMove, lastMove);
                        updateHH(pos.side, depth, currMove, quiets, quietsCount);
                    }
                    break;
                }
            }
        }
        else
            undo(undoer, currMove);
    }

    //// Check for checkmate /
    if (moveSearched == 0)
        return inCheck ? matedIn(ply) : 8 - (nodes & 7); // Randomize draw score so that we try to explore different lines
    U8 ttStoreFlag = hashINVALID;
    if (bestScore >= beta)
        ttStoreFlag = hashLOWER;
    else
    {
        if (alpha != origAlpha)
            ttStoreFlag = hashEXACT;
        else
            ttStoreFlag = hashUPPER;
    }
    if (!stopped && !excludedMove)
        writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply);

    return bestScore;
}

Score Game::quiescence(Score alpha, Score beta)
{

    bool inCheck = pos.inCheck();
    if (inCheck)
        if (isRepetition())
            return 8 - (nodes & 7); // Randomize draw score so that we try to explore different lines

    Score standPat = evaluate();

    if (ply >= maxPly - 1)
        return standPat;

    if (!inCheck)
    {
        if (standPat >= beta)
            return beta;
        Score bigDelta = (egValues[R] + egValues[N]) * isPromotion(pos.lastMove) + egValues[Q];
        if (standPat < alpha - bigDelta)
            return alpha;
        if (alpha < standPat)
            alpha = standPat;
    }

    // Generate moves
    MoveList moveList;

    inCheck ? generateMoves(moveList, ply) : generateCaptures(moveList, ply);

    UndoInfo undoer = UndoInfo(pos);
    U16 moveCount = 0;
    for (int i = 0; i < moveList.count; i++)
    {
        Move move = onlyMove(moveList.moves[i]);
        // SEE pruning
        if (moveCount && getScore(moveList.moves[i]) < GOODCAPTURESCORE)
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

    SStack ss[maxPly];
    for (int i = 0; i < maxPly; i++)
        ss[i].wipe();

    Score delta = ASPIRATIONWINDOW;
    nodes = 0ULL;
    stopped = false;
    ply = 0;
    seldepth = 0;

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

    // Clear history, killer and counter move tables
    memset(historyTable, 0, sizeof(historyTable));
    memset(killerTable, 0, sizeof(killerTable));
    memset(counterMoveTable, 0, sizeof(counterMoveTable));

    // Clear pv len and pv table
    memset(pvLen, 0, sizeof(pvLen));
    memset(pvTable, 0, sizeof(pvTable));

    Score alpha = -infinity;
    Score beta = infinity;

    // Clear evaluation hash table
    for (U64 i = 0; i < (evalHashSize); i++)
        evalHash[i].score = -infinity;

    // Always compute a depth 1 search
    rootDelta = 2 * infinity;
    currSearch = 1;

    Score score = search(-infinity, infinity, 1, ss);
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
            alpha = std::max(S32(-infinity), score - delta);
            beta = std::min(S32(infinity), score + delta);
        }
        while (true)
        {
            seldepth = 0; // Reset seldepth
            ply = 0;
            S64 locNodes = nodes;                        // Save nodes for nps calculation
            U64 timer1 = getTime64();                    // Save time for nps calculation
            score = search(alpha, beta, currSearch, ss); // Search at depth currSearch
            if (stopped)
                goto bmove;
            bestMove = pvTable[0][0];
            U64 timer2 = getTime64();

            if (score <= alpha)
            {
                beta = (alpha + beta) / 2;
                alpha = std::max(S32(-infinity), score - delta);

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