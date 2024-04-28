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

static inline void clearExcludedMove(SStack *stack, Ply ply, Depth depth)
{
    if (ply < maxPly - depth)
    {
        stack[ply + depth].excludedMove = 0;
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

    const bool RootNode = ply == 0;
    const bool PVNode = (beta - alpha) > 1;

    Score eval;
    Move excludedMove = ss->excludedMove;
    bool improving = true;

    // Guard from pvlen editing when in singular extension
    if (!excludedMove)
    {
        pvLen[ply] = ply;
    }

    // Update seldepth
    seldepth = std::max(Ply(seldepth), Ply(ply + 1));

    bool inCheck = isCheck(pos.lastMove) || pos.inCheck();

    // Draws - mate distance
    if (!RootNode)
    {
        if (isRepetition() || pos.fiftyMove >= 100 || pos.insufficientMaterial())
            return 4 - (nodes & 7); // Randomize draw score so that we try to explore different lines

        // Mate distance pruning
        alpha = std::max(alpha, matedIn(ply));
        beta = std::min(beta, mateIn(ply + 1));
        if (alpha >= beta)
            return alpha;
    }

    //// TT probe
    ttEntry *tte = probeTT(pos.hashKey);
    const bool ttHit = !excludedMove && tte;
    Score ttScore = ttHit ? tte->score : noScore;
    const PackedMove ttMove = ttHit ? tte->bestMove : 0;
    const U8 ttFlags = ttHit ? tte->flags : hashNONE;
    const U8 ttBound = ttFlags & 3;

    if (ttScore != noScore)
    {
        // Account for mate values
        ttScore += ply * (ttScore < -mateValue);
        ttScore -= ply * (ttScore > mateValue);
        if (!PVNode && tte->depth >= depth)
        {
            if (ttBound == hashEXACT)
                return ttScore;
            if ((ttBound == hashLOWER))
                alpha = std::max(alpha, ttScore);
            else if ((ttBound == hashUPPER))
                beta = std::min(beta, ttScore);
            if (alpha >= beta)
                return ttScore;
        }
    }

    const bool ttPv = PVNode || (ttHit && (ttFlags & hashPVMove));

    // Quiescence drop
    if (depth <= 0)
        return quiescence(alpha, beta);

    // IIR by Ed SchrÃ¶der
    // http://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=64085e3396554f0fba414404445b3120
    if (depth >= IIRdepth && ttBound == hashNONE && !PVNode)
        depth--;

    // Clear killers and exclude move
    clearNextPlyKillers(ply, 1);
    clearExcludedMove(ss, ply, 1);

    // Initialize the undoer
    UndoInfo undoer = UndoInfo(pos);

    if (inCheck || excludedMove)
    {
        ss->staticEval = eval = noScore;
        improving = false;
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
    else
    {
        eval = ss->staticEval = evaluate();
        // Store the eval in the TT if not in exclusion mode (in which we might already have the entry)
        if (!excludedMove)
            writeTT(pos.hashKey, noScore, eval, 0, hashNONE, 0, ply);
    }

    // Calculate the improving flag
    if (ply >= 2 && (ss - 2)->staticEval != noScore)
        improving = ss->staticEval > (ss - 2)->staticEval;
    else if (ply >= 4 && (ss - 4)->staticEval != noScore)
        improving = ss->staticEval > (ss - 4)->staticEval;
    else
        improving = true; // Since improving makes the pruning more conservative, we default to true

    // Pruning time
    if (!PVNode)
    {

        // RFP
        if (depth <= RFPDepth && abs(eval) < mateScore && eval - futilityMargin(depth, improving) >= beta)
            return eval;

        // Null move pruning
        if (ss->staticEval >= beta + nmpBias &&
            eval >= beta &&
            (ss - 1)->move != noMove &&
            depth >= 3 &&
            isOk(pos.lastMove) &&
            okToReduce(pos.lastMove) && pos.hasNonPawns() &&
            ply >= nmpPlies)
        {

            // make null move
            makeNullMove();
            ss->move = noMove;

            Depth R = nmpQ1 + (depth / nmpDepthDivisor) + std::min((eval - beta) / nmpScoreDivisor, nmpQ2);
            Score nullScore = -search(-beta, -beta + 1, depth - R, ss + 1);

            undoNullMove(undoer);

            if (stopped)
                return 0;

            if (nullScore >= beta)
            {
                if (nullScore >= mateValue)
                    nullScore = beta;
                if (depth < 15)
                    return nullScore;

                // If the null move failed high, we can try a reduced search when depth is high. This is to avoid zugzwang positions
                nmpPlies = ply + (depth - R) * 2 / 3;
                Score verification = search(beta - 1, beta, depth - R, ss);
                nmpPlies = 0;

                if (verification >= beta)
                    return nullScore;
            }
        }

        // TODO: Razoring
        if (depth <= razorDepth && eval + razorQ1 + razorQ2 * (depth - 1) <= alpha)
        {
            const Score razorScore = quiescence(alpha, beta);
            if (razorScore <= alpha)
                return razorScore;
        }
    }

skipPruning:

    // Search
    Score origAlpha = alpha;
    Score bestScore = noScore;
    Move bestMove = noMove;
    U16 moveSearched = 0;
    bool skipQuiets = false;

    pvLen[ply] = ply;

    Move quiets[256];
    U16 quietsCount = 0;

    MoveList moveList;
    generateMoves(moveList, ply); // Already sorted, except for ttMove
    //// Sort ttMove
    if (ttMove)
        sortTTUp(moveList, ttMove);
    Move unpackedTTMove = onlyMove(moveList.moves[0]);

    // Iterate through moves
    for (int i = 0; i < moveList.count; i++)
    {
        Move currMove = onlyMove(moveList.moves[i]);
        if (!currMove || currMove == excludedMove)
            continue;

        const bool isQuiet = okToReduce(currMove);

        if (skipQuiets && isQuiet)
            continue;

        S32 currMoveScore = getScore(moveList.moves[i]) - 16384;

        if (ply && pos.hasNonPawns() && bestScore > noScore)
        {
            const Depth lmrDepth = Depth(std::max(0, depth - reduction(depth, moveSearched, PVNode, improving) + std::clamp((currMoveScore / 16384), -1, 1)));

            if (!skipQuiets && !inCheck && !PVNode)
            {
                // Movecount pruning
                if (moveSearched >= lmpMargin[depth][improving])
                    skipQuiets = true;
                // Futility pruning
                if (moveSearched && lmrDepth < 11 && ss->staticEval + futPruningMultiplier + futPruningAdd * lmrDepth <= alpha)
                    skipQuiets = true;
            }
        }

        Depth extension = 0;
        if (ply < currSearch * 2 && depth >= 6 && currMove == unpackedTTMove && !excludedMove && (ttBound & hashLOWER) && abs(ttScore) < mateScore && tte->depth >= depth - 3 && !RootNode)
        {
            const Score singularBeta = ttScore - singularDepthMultiplier * depth;
            const Depth singularDepth = (depth - 1) / 2;

            ss->excludedMove = ttMove;
            const Score singularScore = search(singularBeta - 1, singularBeta, singularDepth, ss);
            ss->excludedMove = noMove;

            if (singularScore < singularBeta)
            {
                extension = 1;
                // Limit search explosion
                if (!PVNode && singularScore < singularBeta - 15 && ss->doubleExtensions <= 11)
                {
                    extension = 2 + (okToReduce(ttMove) && singularScore < singularBeta - 100);
                    ss->doubleExtensions = (ss - 1)->doubleExtensions + 1;
                    depth += depth < 10; // ?????
                }
            }
            else if (singularBeta >= beta)
                return singularBeta;
            else if (ttScore >= beta)
                extension = -2; // Negative extension (apparently good)
        }

        Depth newDepth = depth - 1 + extension;

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

            if (moveSearched > PVNode * 3 && depth >= 3 && (isQuiet || !ttPv))
            {
                Depth R = reduction(depth, moveSearched, PVNode, improving);
                // If move is highly tactical, reduce it less
                if (sameMovePos(currMove, killerTable[0][ply]) || sameMovePos(currMove, killerTable[1][ply]) || sameMovePos(currMove, counterMoveTable[movePiece(ss->move)][moveTarget(ss->move)]))
                    R -= 1;

                if (ttPv)
                    R -= 1;

                if (givesCheck)
                    R -= 1;

                R -= std::clamp((currMoveScore / 16384), -1, 1);

                R = std::max(Depth(0), R);
                R = std::min(Depth(newDepth - Depth(1)), R);

                Depth reducedDepth = newDepth - R;
                // Search at reduced depth
                score = -search(-alpha - 1, -alpha, reducedDepth, ss + 1);
                // If failed high on reduced search, research at full depth
                if (score > alpha && R)
                    score = -search(-alpha - 1, -alpha, newDepth, ss + 1);
            }
            else if (!PVNode || moveSearched)
                score = -search(-alpha - 1, -alpha, newDepth, ss + 1);

            // Pvsearch
            if (PVNode && (!moveSearched || score > alpha))
            {
                score = -search(-beta, -alpha, newDepth, ss + 1);
            }

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
                        // Copy the pv from the next ply
                        for (int j = ply + 1; j < pvLen[ply + 1]; j++)
                            pvTable[ply][j] = pvTable[ply + 1][j];
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
        return inCheck ? matedIn(ply) : 4 - (nodes & 7); // Randomize draw score so that we try to explore different lines
    U8 ttStoreFlag = hashNONE;
    if (bestScore >= beta)
        ttStoreFlag = hashLOWER;
    else
    {
        if (alpha != origAlpha)
            ttStoreFlag = hashEXACT;
        else
            ttStoreFlag = hashUPPER;
    }
    // If pv, add the ttPv flag
    if (ttPv)
        ttStoreFlag |= hashPVMove;
    if (!stopped && !excludedMove)
        writeTT(pos.hashKey, bestScore, ss->staticEval, depth, ttStoreFlag, bestMove, ply);

    return bestScore;
}

Score Game::quiescence(Score alpha, Score beta)
{
    bool inCheck = pos.inCheck();
    if (inCheck)
        if (isRepetition())
            return 4 - (nodes & 7); // Randomize draw score so that we try to explore different lines

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

    // Clear history, killer and counter move tables
    memset(historyTable, 0, sizeof(historyTable));
    memset(killerTable, 0, sizeof(killerTable));
    memset(counterMoveTable, 0, sizeof(counterMoveTable));

    // Clear pv len and pv table
    memset(pvLen, 0, sizeof(pvLen));
    memset(pvTable, 0, sizeof(pvTable));

    Score alpha = noScore;
    Score beta = infinity;

    // Clear evaluation hash table
    for (U64 i = 0; i < (evalHashSize); i++)
        evalHash[i].score = noScore;

    // Always compute a depth 1 search
    rootDelta = 2 * infinity;
    currSearch = 1;

    Score score = search(noScore, infinity, 1, ss);
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
            score = search(alpha, beta, currSearch, ss); // Search at depth currSearch
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