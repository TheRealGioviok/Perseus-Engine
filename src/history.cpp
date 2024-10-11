#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"
#include "Game.h"
#include <algorithm>
// history table
S32 historyTable[2][NUM_SQUARES * NUM_SQUARES];

// capture history table
S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][6];

// counter move table
Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

// Correction History
S32 pawnsCorrHist[2][CORRHISTSIZE]; // stm - hash
S32 nonPawnsCorrHist[2][2][CORRHISTSIZE]; // stm - side - hash


void updateHistoryMove(bool side, Move move, S32 delta) {
    S32 *current = &historyTable[side][indexFromTo(moveSource(move),moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateCaptureHistory(Move move, S32 delta) {
    Piece captured = moveCapture(move);
    S32 *current = &captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][captured == NOPIECE ? P : captured % 6]; // account for promotion
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateContHistOffset(SStack* ss, const Move move, const S32 delta, const S32 offset){
    S32 *current = &(ss - offset)->contHistEntry[indexPieceTo(movePiece(move), moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateContHist(SStack* ss, const Move move, const S32 delta){
    updateContHistOffset(ss, move, delta, 1);
    updateContHistOffset(ss, move, delta, 2);
}

void updateHH(SStack* ss, bool side, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount) {
    const S32 delta = stat_bonus(depth);
    if (okToReduce(bestMove)) {
        // If bestMove is not noisy, we reduce the bonus of all other moves and increase the bonus of the bestMove
        updateHistoryMove(side, bestMove, delta);
        updateContHist(ss, bestMove, delta);
        for (int i = 0; i < quietsCount; i++) {
            if (quietMoves[i] == bestMove) continue;
            updateHistoryMove(side, quietMoves[i], -delta);
            updateContHist(ss, quietMoves[i], -delta);
        }
    }
    else {
        // If bestMove is noisy, we only reduce the bonus of all other noisy moves
        updateCaptureHistory(bestMove, delta);
    }
    for (int i = 0; i < noisyCount; i++) {
        if (noisyMoves[i] == bestMove) continue;
        updateCaptureHistory(noisyMoves[i], -delta);
    }
}


Score correctStaticEval(Position& pos, const Score eval) {
    const bool side = pos.side;

    const S32 pawnBonus = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE];

    const S32 nonPawnBonus = (
        nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE] 
        + nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE]
    ) / 2;

    const S32 bonus = pawnBonus + nonPawnBonus;
    const S32 corrected = eval + bonus / CORRHISTSCALE;

    return static_cast<Score>(std::clamp(corrected, -mateValue, +mateValue));
}

void updateSingleCorrHist(S32& entry, const S32 bonus, const S32 weight){
    entry = (entry * (256 - weight) + bonus * weight) / 256;
    entry = static_cast<Score>(std::clamp(entry, -MAXCORRHIST, MAXCORRHIST));
}

void updateCorrHist(Position& pos, const Score bonus, const Depth depth){
    const bool side = pos.side;
    const S32 scaledBonus = bonus * CORRHISTSCALE; 
    const S32 weight = std::min(1 + depth, 16);

    auto& pawnEntry = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE];
    auto& wNonPawnEntry = nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE];
    auto& bNonPawnEntry = nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE];
    
    updateSingleCorrHist(pawnEntry, scaledBonus, weight);
    updateSingleCorrHist(wNonPawnEntry, scaledBonus, weight);
    updateSingleCorrHist(bNonPawnEntry, scaledBonus, weight);
}