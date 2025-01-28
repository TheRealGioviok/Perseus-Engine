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

    const S32 pawnBonus = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE] ;

    const S32 nonPawnBonus = 
        + nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE] 
        + nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE]
    ;

    auto const& k = pos.ptKeys;

    const S32 tripletBonus = 
        + tripletCorrHist[0][side][(k[K] ^ k[P] ^ k[N]) % CORRHISTSIZE]
        + tripletCorrHist[1][side][(k[K] ^ k[P] ^ k[B]) % CORRHISTSIZE]
        + tripletCorrHist[2][side][(k[K] ^ k[P] ^ k[R]) % CORRHISTSIZE]
        + tripletCorrHist[3][side][(k[K] ^ k[P] ^ k[Q]) % CORRHISTSIZE]
        + tripletCorrHist[4][side][(k[K] ^ k[N] ^ k[B]) % CORRHISTSIZE]
        + tripletCorrHist[5][side][(k[K] ^ k[N] ^ k[R]) % CORRHISTSIZE]
        + tripletCorrHist[6][side][(k[K] ^ k[N] ^ k[Q]) % CORRHISTSIZE]
        + tripletCorrHist[7][side][(k[K] ^ k[B] ^ k[R]) % CORRHISTSIZE]
        + tripletCorrHist[8][side][(k[K] ^ k[B] ^ k[Q]) % CORRHISTSIZE]
        + tripletCorrHist[9][side][(k[K] ^ k[R] ^ k[Q]) % CORRHISTSIZE]
    ;

    const S32 bonus = pawnBonus + nonPawnBonus + tripletBonus;
    const S32 corrected = eval + bonus / CORRHISTSCALE;

    return static_cast<Score>(std::clamp(corrected, -mateValue, +mateValue));
}

static inline void updateSingleCorrHist(S32& entry, const S32 bonus, const S32 weight){
    entry = (entry * (256 - weight) + bonus * weight) / 256;
    entry = static_cast<Score>(std::clamp(entry, -MAXCORRHIST, MAXCORRHIST));
}

void updateCorrHist(Position& pos, const Score bonus, const Depth depth){
    const bool side = pos.side;
    const S32 scaledBonus = bonus * CORRHISTSCALE; 
    const S32 weight = std::min(1 + depth, 16);

    auto& bb = pos.bitboards;
    auto& occ = pos.occupancies;

    const S32 ptCounts[5] = {
        popcount(bb[P] ^ bb[p]),
        popcount(bb[N] ^ bb[n]),
        popcount(bb[B] ^ bb[b]),
        popcount(bb[R] ^ bb[r]),
        popcount(bb[Q] ^ bb[q]),
    };

    auto const& k = pos.ptKeys;

    auto& pawnEntry = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE];
    auto& wNonPawnEntry = nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE];
    auto& bNonPawnEntry = nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE];

    updateSingleCorrHist(pawnEntry, scaledBonus * (ptCounts[P]) / 16, weight);
    updateSingleCorrHist(wNonPawnEntry, scaledBonus * (popcount(occ[WHITE] ^ bb[P]) - 1) / 7, weight);
    updateSingleCorrHist(bNonPawnEntry, scaledBonus * (popcount(occ[BLACK] ^ bb[p]) - 1) / 7, weight);

    updateSingleCorrHist(tripletCorrHist[0][side][(k[K] ^ k[P] ^ k[N]) % CORRHISTSIZE], scaledBonus * (ptCounts[P] + ptCounts[N]) / 20, weight);
    updateSingleCorrHist(tripletCorrHist[1][side][(k[K] ^ k[P] ^ k[B]) % CORRHISTSIZE], scaledBonus * (ptCounts[P] + ptCounts[B]) / 20, weight);
    updateSingleCorrHist(tripletCorrHist[2][side][(k[K] ^ k[P] ^ k[R]) % CORRHISTSIZE], scaledBonus * (ptCounts[P] + ptCounts[R]) / 20, weight);
    updateSingleCorrHist(tripletCorrHist[3][side][(k[K] ^ k[P] ^ k[Q]) % CORRHISTSIZE], scaledBonus * (ptCounts[P] + ptCounts[Q]) / 18, weight);
    updateSingleCorrHist(tripletCorrHist[4][side][(k[K] ^ k[N] ^ k[B]) % CORRHISTSIZE], scaledBonus * (ptCounts[N] + ptCounts[B]) / 8, weight);
    updateSingleCorrHist(tripletCorrHist[5][side][(k[K] ^ k[N] ^ k[R]) % CORRHISTSIZE], scaledBonus * (ptCounts[N] + ptCounts[R]) / 8, weight);
    updateSingleCorrHist(tripletCorrHist[6][side][(k[K] ^ k[N] ^ k[Q]) % CORRHISTSIZE], scaledBonus * (ptCounts[N] + ptCounts[Q]) / 6, weight);
    updateSingleCorrHist(tripletCorrHist[7][side][(k[K] ^ k[B] ^ k[R]) % CORRHISTSIZE], scaledBonus * (ptCounts[B] + ptCounts[R]) / 8, weight);
    updateSingleCorrHist(tripletCorrHist[8][side][(k[K] ^ k[B] ^ k[Q]) % CORRHISTSIZE], scaledBonus * (ptCounts[B] + ptCounts[Q]) / 6, weight);
    updateSingleCorrHist(tripletCorrHist[9][side][(k[K] ^ k[R] ^ k[Q]) % CORRHISTSIZE], scaledBonus * (ptCounts[R] + ptCounts[Q]) / 6, weight);
}