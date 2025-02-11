#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"
#include "Game.h"
#include <algorithm>

// history table
S32 historyTable[2][4][NUM_SQUARES * NUM_SQUARES];

// capture history table
S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][6];

// counter move table
Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

// Correction History
S32 pawnsCorrHist[2][CORRHISTSIZE]; // stm - hash
S32 nonPawnsCorrHist[2][2][CORRHISTSIZE]; // stm - side - hash
S32 tripletCorrHist[10][2][CORRHISTSIZE]; // stm - hash

static inline void updateHistoryMove(const bool side, const BitBoard threats, const Move move, const S32 delta) {
    S32 *current = &historyTable[side][getThreatsIndexing(threats, move)][indexFromTo(moveSource(move),moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateCaptureHistory(Move move, S32 delta) {
    Piece captured = moveCapture(move);
    S32 *current = &captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][captured == NOPIECE ? P : captured % 6]; // account for promotion
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateContHistOffset(SStack* ss, const Move move, const S32 delta, const S32 offset){
    S32 *current = &(ss - offset)->contHistEntry[indexPieceTo(movePiece(move), moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateContHist(SStack* ss, const Move move, const S32 delta){
    updateContHistOffset(ss, move, delta, 1);
    updateContHistOffset(ss, move, delta, 2);
    updateContHistOffset(ss, move, delta, 4);
}

void updateHH(SStack* ss, bool side, BitBoard threats, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount) {
    const S32 delta = stat_bonus(depth);
    if (okToReduce(bestMove)) {
        // If bestMove is not noisy, we reduce the bonus of all other moves and increase the bonus of the bestMove
        updateHistoryMove(side, threats, bestMove, delta);
        updateContHist(ss, bestMove, delta);
        for (int i = 0; i < quietsCount; i++) {
            if (quietMoves[i] == bestMove) continue;
            updateHistoryMove(side, threats, quietMoves[i], -delta);
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
    auto const& k = pos.ptKeys;

    const S32 bonus = 
        + pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE]           * pawnCorrWeight()
        + (
            + nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE]
            + nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE]
        )                                                               * nonPawnCorrWeight()
        + tripletCorrHist[0][side][(k[K] ^ k[P] ^ k[N]) % CORRHISTSIZE] * T0CorrWeight()
        + tripletCorrHist[1][side][(k[K] ^ k[P] ^ k[B]) % CORRHISTSIZE] * T1CorrWeight()
        + tripletCorrHist[2][side][(k[K] ^ k[P] ^ k[R]) % CORRHISTSIZE] * T2CorrWeight()
        + tripletCorrHist[3][side][(k[K] ^ k[P] ^ k[Q]) % CORRHISTSIZE] * T3CorrWeight()
        + tripletCorrHist[4][side][(k[K] ^ k[N] ^ k[B]) % CORRHISTSIZE] * T4CorrWeight()
        + tripletCorrHist[5][side][(k[K] ^ k[N] ^ k[R]) % CORRHISTSIZE] * T5CorrWeight()
        + tripletCorrHist[6][side][(k[K] ^ k[N] ^ k[Q]) % CORRHISTSIZE] * T6CorrWeight()
        + tripletCorrHist[7][side][(k[K] ^ k[B] ^ k[R]) % CORRHISTSIZE] * T7CorrWeight()
        + tripletCorrHist[8][side][(k[K] ^ k[B] ^ k[Q]) % CORRHISTSIZE] * T8CorrWeight()
        + tripletCorrHist[9][side][(k[K] ^ k[R] ^ k[Q]) % CORRHISTSIZE] * T9CorrWeight()
    ;

    const S32 corrected = eval + bonus / (CORRHISTSCALE * CORRECTIONGRANULARITY);

    return static_cast<Score>(std::clamp(corrected, -mateValue, +mateValue));
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
    const S32 weight = std::min(1 + depth, 16);

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