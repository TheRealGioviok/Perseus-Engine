#pragma once
#include "BBmacros.h"
#include "types.h"
#include "constants.h"
#include "Position.h"

// history table
extern S16 historyTable[2][NUM_SQUARES * NUM_SQUARES][4];

// capture history table
extern S16 captureHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES / 2][4]; // The color of the captured piece is always the opposite of the color of the moving piece

// counter move table
extern Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
extern S16 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

constexpr S32 CORRHISTSIZE = 16384;
constexpr S32 CORRHISTSCALE = 256;
constexpr S32 CORRECTIONGRANULARITY = 1024;

// static (read‑only during search) correction histories
extern S32 pawnsCorrHist[2][CORRHISTSIZE];
extern S32 nonPawnsCorrHist[2][2][CORRHISTSIZE];
extern S32 tripletCorrHist[10][2][CORRHISTSIZE];

// dynamic (write‑only during search) correction histories
extern S32 pawnsCorrHistDyn[2][CORRHISTSIZE];
extern S32 nonPawnsCorrHistDyn[2][2][CORRHISTSIZE];
extern S32 tripletCorrHistDyn[10][2][CORRHISTSIZE];

struct SStack {
    Move excludedMove = 0;
    Score staticEval = 0;
    Move move = 0;
    Move killers[2] = {0, 0};
    S16* contHistEntry = continuationHistoryTable[0];

    SStack() {
        excludedMove = noMove;
        staticEval = 0;
        move = noMove;
        killers[0] = killers[1] = noMove;
        contHistEntry = continuationHistoryTable[0];
    }

    void wipe() {
        excludedMove = noMove;
        staticEval = 0;
        move = noMove;
        killers[0] = killers[1] = noMove;
        contHistEntry = continuationHistoryTable[0];
    }
};

inline S32 indexFromTo(Square from, Square to) {
    return from * 64 + to;
}

inline S32 indexPieceTo(Piece piece, Square to) {
    return piece * 64 + (to^56); // So that P to a8 (0 if we don't ^56) is not the same as indexPieceTo(nullMove), which is instead the same as p to a8, which is impossible.
}

static inline S32 getThreatsIndexing(const BitBoard threats, const Move move){
    const BitBoard source = squareBB(moveSource(move));
    const BitBoard target = squareBB(moveTarget(move));
    return ((source & threats)>0) + 2*((target & threats)>0);
}

static inline S32 statBonus(S32 depth){
    return std::min(maxHistoryBonus(), depth * depth * historyBonusQuadDepth() + depth * historyBonusLinDepth() + historyBonusOffset());
}

static inline S32 statMalus(S32 depth){
    return std::min(maxHistoryMalus(), depth * depth * historyMalusQuadDepth() + depth * historyMalusLinDepth() + historyMalusOffset());
}

#define MAXHISTORYABS 16384LL
void updateHH(SStack* ss, bool side, BitBoard threats, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount);

// correctStaticEval remains unchanged: it reads only from the static tables
template <bool doCorrHist>
Score correctStaticEval(Position& pos, Score eval, Score& dynEval) {
    // fifty‐move decay
    eval = static_cast<Score>(std::clamp(
        (eval * (220 - pos.fiftyMove) / 220),
        -mateValue, +mateValue));

    if constexpr (doCorrHist) {
        const bool side = pos.side;
        auto const& k = pos.ptKeys;
        S32 bonus =
            + pawnsCorrHist[side][pos.pawnHashKey % CORRHISTSIZE] * pawnCorrWeight()
            + (
                + nonPawnsCorrHist[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE]
                + nonPawnsCorrHist[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE]
              ) * nonPawnCorrWeight();

        S32 dynBonus = pawnsCorrHistDyn[side][pos.pawnHashKey % CORRHISTSIZE] * pawnCorrWeight()
            + (
                + nonPawnsCorrHistDyn[side][WHITE][pos.nonPawnKeys[WHITE] % CORRHISTSIZE]
                + nonPawnsCorrHistDyn[side][BLACK][pos.nonPawnKeys[BLACK] % CORRHISTSIZE]
              ) * nonPawnCorrWeight();

        // triplet weights T0CorrWeight()+i// triplets
        static constexpr S32 TRIPIDX[10][3] = {
            {K,P,N},{K,P,B},{K,P,R},{K,P,Q},
            {K,N,B},{K,N,R},{K,N,Q},
            {K,B,R},{K,B,Q},
            {K,R,Q}
        };
        static constexpr S32 weights[10] = {
            T0CorrWeight(), T1CorrWeight(), T2CorrWeight(), T3CorrWeight(),
            T4CorrWeight(),T5CorrWeight(),T6CorrWeight(),
            T7CorrWeight(),T8CorrWeight(),
            T9CorrWeight()
        };
        for (int t = 0; t < 10; ++t){
            int idx = (k[TRIPIDX[t][0]]
                     ^ k[TRIPIDX[t][1]]
                     ^ k[TRIPIDX[t][2]])
                     % CORRHISTSIZE;
            bonus += tripletCorrHist[t][side][idx] * weights[t];
            dynBonus += tripletCorrHistDyn[t][side][idx] * weights[t];
        }
        dynEval = eval + dynBonus / (CORRHISTSCALE * CORRECTIONGRANULARITY);
        return eval + bonus / (CORRHISTSCALE * CORRECTIONGRANULARITY);
    }
    else {
        dynEval = eval;
        return eval;
    }
}

void updateCorrHist(Position& pos, const Score bonus, const Depth depth);

static inline void updateHistoryMove(const bool side, const BitBoard threats, const Move move, const S32 delta) {
    S16 *current = &historyTable[side][indexFromTo(moveSource(move), moveTarget(move))][getThreatsIndexing(threats, move)];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateCaptureHistory(Move move, const BitBoard threats, S32 delta) {
    Piece captured = moveCapture(move);
    S16 *current = &captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][captured == NOPIECE ? P : captured % 6][getThreatsIndexing(threats, move)]; // account for promotion
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateContHistOffset(SStack* ss, const Move move, const S32 delta, const S32 offset){
    S16 *current = &(ss - offset)->contHistEntry[indexPieceTo(movePiece(move), moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateContHist(SStack* ss, const Move move, const S32 delta){
    updateContHistOffset(ss, move, delta, 1);
    updateContHistOffset(ss, move, delta, 2);
    updateContHistOffset(ss, move, delta, 4);
}