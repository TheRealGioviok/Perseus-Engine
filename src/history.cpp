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
    auto& bb = pos.bitboards;
    auto& occ = pos.occupancies;

    const S32 pawnBonus = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE] * (popcount(bb[P] ^ bb[p])) / 16;

    const S32 nonPawnBonus = 
        + nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE] * (popcount(occ[WHITE]^bb[P]) - 1) / 7
        + nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE] * (popcount(occ[BLACK]^bb[p]) - 1) / 7
    ;

    auto const& k = pos.ptKeys;

    const S32 tripletBonus = 
        + tripletCorrHist[0][side][(k[K] ^ k[P] ^ k[N]) % CORRHISTSIZE] * (popcount(bb[P] ^ bb[p] ^ bb[N] ^ bb[n])) / 20
        + tripletCorrHist[1][side][(k[K] ^ k[P] ^ k[B]) % CORRHISTSIZE] * (popcount(bb[P] ^ bb[p] ^ bb[B] ^ bb[b])) / 20
        + tripletCorrHist[2][side][(k[K] ^ k[P] ^ k[R]) % CORRHISTSIZE] * (popcount(bb[P] ^ bb[p] ^ bb[R] ^ bb[r])) / 20
        + tripletCorrHist[3][side][(k[K] ^ k[P] ^ k[Q]) % CORRHISTSIZE] * (popcount(bb[P] ^ bb[p] ^ bb[Q] ^ bb[q])) / 20
        + tripletCorrHist[4][side][(k[K] ^ k[N] ^ k[B]) % CORRHISTSIZE] * (popcount(bb[N] ^ bb[n] ^ bb[B] ^ bb[b])) / 8
        + tripletCorrHist[5][side][(k[K] ^ k[N] ^ k[R]) % CORRHISTSIZE] * (popcount(bb[N] ^ bb[n] ^ bb[R] ^ bb[r])) / 8
        + tripletCorrHist[6][side][(k[K] ^ k[N] ^ k[Q]) % CORRHISTSIZE] * (popcount(bb[N] ^ bb[n] ^ bb[Q] ^ bb[q])) / 6
        + tripletCorrHist[7][side][(k[K] ^ k[B] ^ k[R]) % CORRHISTSIZE] * (popcount(bb[B] ^ bb[b] ^ bb[R] ^ bb[r])) / 8
        + tripletCorrHist[8][side][(k[K] ^ k[B] ^ k[Q]) % CORRHISTSIZE] * (popcount(bb[B] ^ bb[B] ^ bb[Q] ^ bb[q])) / 6
        + tripletCorrHist[9][side][(k[K] ^ k[R] ^ k[Q]) % CORRHISTSIZE] * (popcount(bb[R] ^ bb[r] ^ bb[Q] ^ bb[q])) / 6
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

    auto const& k = pos.ptKeys;

    auto& pawnEntry = pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE];
    auto& wNonPawnEntry = nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE];
    auto& bNonPawnEntry = nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE];

    updateSingleCorrHist(pawnEntry, scaledBonus, weight);
    updateSingleCorrHist(wNonPawnEntry, scaledBonus, weight);
    updateSingleCorrHist(bNonPawnEntry, scaledBonus, weight);

    updateSingleCorrHist(tripletCorrHist[0][side][(k[K] ^ k[P] ^ k[N]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[1][side][(k[K] ^ k[P] ^ k[B]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[2][side][(k[K] ^ k[P] ^ k[R]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[3][side][(k[K] ^ k[P] ^ k[Q]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[4][side][(k[K] ^ k[N] ^ k[B]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[5][side][(k[K] ^ k[N] ^ k[R]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[6][side][(k[K] ^ k[N] ^ k[Q]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[7][side][(k[K] ^ k[B] ^ k[R]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[8][side][(k[K] ^ k[B] ^ k[Q]) % CORRHISTSIZE], scaledBonus, weight);
    updateSingleCorrHist(tripletCorrHist[9][side][(k[K] ^ k[R] ^ k[Q]) % CORRHISTSIZE], scaledBonus, weight);
}