#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"
#include "Game.h"
#include <algorithm>

// history table
S32 historyTable[2][NUM_SQUARES * NUM_SQUARES][4];

// capture history table
S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][6][4];

// counter move table
Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

// Correction History
S32 pawnsCorrHist[2][CORRHISTSIZE]; // stm - hash
S32 nonPawnsCorrHist[2][2][CORRHISTSIZE]; // stm - side - hash
S32 tripletCorrHist[10][2][CORRHISTSIZE]; // stm - hash

void updateHH(SStack* ss, bool side, BitBoard threats, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount) {
    const S32 bonus = statBonus(depth);
    const S32 malus = statMalus(depth);
    if (okToReduce(bestMove)) {
        // If bestMove is not noisy, we reduce the bonus of all other moves and increase the bonus of the bestMove
        updateHistoryMove(side, threats, bestMove, bonus);
        updateContHist(ss, bestMove, bonus);
        for (int i = 0; i < quietsCount; i++) {
            if (quietMoves[i] == bestMove) continue;
            updateHistoryMove(side, threats, quietMoves[i], -malus);
            updateContHist(ss, quietMoves[i], -malus);
        }
    }
    else {
        // If bestMove is noisy, we only reduce the bonus of all other noisy moves
        updateCaptureHistory(bestMove, threats, bonus);
    }
    for (int i = 0; i < noisyCount; i++) {
        if (noisyMoves[i] == bestMove) continue;
        updateCaptureHistory(noisyMoves[i], threats, -malus);
    }
}

static inline void updateSingleCorrHist(S32& entry, const S32 bonus, const S32 weight){
    const S32 MAXCORRHIST = CORRHISTSCALE * MAXCORRHISTUNSCALED();
    const S32 MAXCORRHISTUPDATE = MAXCORRHIST * MAXCORRHISTMILLIUPDATE() / CORRECTIONGRANULARITY;

    S32 newValue = (entry * (256 - weight) + bonus * weight) / 256;
    newValue = std::clamp(newValue, entry - MAXCORRHISTUPDATE, entry + MAXCORRHISTUPDATE);
    entry = std::clamp(newValue, -MAXCORRHIST, MAXCORRHIST);
}

void updateCorrHist(Position& pos, const Score bonus, const Depth depth){
    const bool side = pos.side;
    const S64 scaledBonus = bonus * CORRHISTSCALE; 
    const S32 weight = 2 * std::min(1 + depth, 16);

    auto const& k = pos.ptKeys;

    auto& pawnEntry = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE];
    auto& wNonPawnEntry = nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE];
    auto& bNonPawnEntry = nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE];

    updateSingleCorrHist(pawnEntry, scaledBonus * pawnCorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(wNonPawnEntry, scaledBonus * nonPawnCorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(bNonPawnEntry, scaledBonus * nonPawnCorrUpdate() / CORRECTIONGRANULARITY, weight);

    updateSingleCorrHist(tripletCorrHist[0][side][(k[K] ^ k[P] ^ k[N]) % CORRHISTSIZE], scaledBonus * T0CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[1][side][(k[K] ^ k[P] ^ k[B]) % CORRHISTSIZE], scaledBonus * T1CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[2][side][(k[K] ^ k[P] ^ k[R]) % CORRHISTSIZE], scaledBonus * T2CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[3][side][(k[K] ^ k[P] ^ k[Q]) % CORRHISTSIZE], scaledBonus * T3CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[4][side][(k[K] ^ k[N] ^ k[B]) % CORRHISTSIZE], scaledBonus * T4CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[5][side][(k[K] ^ k[N] ^ k[R]) % CORRHISTSIZE], scaledBonus * T5CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[6][side][(k[K] ^ k[N] ^ k[Q]) % CORRHISTSIZE], scaledBonus * T6CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[7][side][(k[K] ^ k[B] ^ k[R]) % CORRHISTSIZE], scaledBonus * T7CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[8][side][(k[K] ^ k[B] ^ k[Q]) % CORRHISTSIZE], scaledBonus * T8CorrUpdate() / CORRECTIONGRANULARITY, weight);
    updateSingleCorrHist(tripletCorrHist[9][side][(k[K] ^ k[R] ^ k[Q]) % CORRHISTSIZE], scaledBonus * T9CorrUpdate() / CORRECTIONGRANULARITY, weight);
}
