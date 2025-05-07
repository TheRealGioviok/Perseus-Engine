#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"
#include "Game.h"
#include <algorithm>

// history table
S16 historyTable[2][NUM_SQUARES * NUM_SQUARES][4];

// capture history table
S16 captureHistoryTable[NUM_PIECES * NUM_SQUARES][6][4];

// counter move table
Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
S16 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

// ——— Static tables ———
S32 pawnsCorrHist[2][CORRHISTSIZE];
S32 nonPawnsCorrHist[2][2][CORRHISTSIZE];
S32 tripletCorrHist[10][2][CORRHISTSIZE];

// ——— Dynamic tables ———
S32 pawnsCorrHistDyn[2][CORRHISTSIZE];
S32 nonPawnsCorrHistDyn[2][2][CORRHISTSIZE];
S32 tripletCorrHistDyn[10][2][CORRHISTSIZE];

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

// single‐entry update (identical to before)
static inline void updateSingleCorrHist(
    S32& entry, const S32 bonus, const S32 weight)
{
    const S32 MAXCORRHIST = CORRHISTSCALE * MAXCORRHISTUNSCALED();
    const S32 MAXCORRHISTUPDATE =
        MAXCORRHIST * MAXCORRHISTMILLIUPDATE() / CORRECTIONGRANULARITY;

    S32 newValue =
        (entry * (256 - weight) + bonus * weight) / 256;
    newValue = std::clamp(newValue, entry - MAXCORRHISTUPDATE,
                                     entry + MAXCORRHISTUPDATE);
    entry = std::clamp(newValue, -MAXCORRHIST, +MAXCORRHIST);
}

// redirect updates into the *Dyn tables
void updateCorrHist(Position& pos, const Score bonus, const Depth depth)
{
    const bool side = pos.side;
    const S64 scaledBonus = bonus * CORRHISTSCALE;
    const S32 weight = 2 * std::min(1 + depth, 16);
    auto const& k = pos.ptKeys;

    // pawn
    auto& pawnE = pawnsCorrHistDyn[side]
                             [pos.pawnHashKey % CORRHISTSIZE];
    updateSingleCorrHist(
        pawnE,
        scaledBonus * pawnCorrUpdate() / CORRECTIONGRANULARITY,
        weight);

    // non‑pawn white & black
    for (int c = 0; c < 2; ++c){
        auto& npE = nonPawnsCorrHistDyn[side][c]
                              [pos.nonPawnKeys[c] % CORRHISTSIZE];
        updateSingleCorrHist(
            npE,
            scaledBonus * nonPawnCorrUpdate() / CORRECTIONGRANULARITY,
            weight);
    }

    // triplets
    static constexpr int TRIPIDX[10][3] = {
      {K,P,N},{K,P,B},{K,P,R},{K,P,Q},
      {K,N,B},{K,N,R},{K,N,Q},{K,B,R},
      {K,B,Q},{K,R,Q}
    };
    for (int t = 0; t < 10; ++t){
        auto& te = tripletCorrHistDyn[t][side]
            [(k[TRIPIDX[t][0]]
            ^ k[TRIPIDX[t][1]]
            ^ k[TRIPIDX[t][2]])
            % CORRHISTSIZE];
        const S32 up = scaledBonus * (T0CorrUpdate()+t) / CORRECTIONGRANULARITY;
        updateSingleCorrHist(te, up, weight);
    }
}