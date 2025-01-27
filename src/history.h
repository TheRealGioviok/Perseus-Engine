#pragma once
#include "BBmacros.h"
#include "types.h"
#include "constants.h"
#include "Position.h"

// Forward declaration time
struct Position;

// history table
extern S32 historyTable[2][4][NUM_SQUARES * NUM_SQUARES];

// capture history table
extern S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES / 2]; // The color of the captured piece is always the opposite of the color of the moving piece

// counter move table
extern Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
extern S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

#define CORRHISTSIZE 16384
#define CORRHISTSCALE 256
#define MAXCORRHIST (CORRHISTSCALE * 32)
// Correction History
extern S32 pawnsCorrHist[2][CORRHISTSIZE];
extern S32 nonPawnsCorrHist[2][2][CORRHISTSIZE];

extern S32 tripletCorrHist[10][2][CORRHISTSIZE]; // 10 is the number of triplet indexing of type KXY, with X and Y satisfying X != K, Y != K, X != Y

struct SStack {
    Move excludedMove = 0;
    Score staticEval = 0;
    Move move = 0;
    S16 doubleExtensions = 0;
    Move killers[2] = {0, 0};
    S32* contHistEntry = continuationHistoryTable[0];

    SStack() {
        excludedMove = noMove;
        staticEval = 0;
        move = noMove;
        doubleExtensions = 0;
        killers[0] = killers[1] = noMove;
        contHistEntry = continuationHistoryTable[0];
    }

    void wipe() {
        excludedMove = noMove;
        staticEval = 0;
        move = noMove;
        doubleExtensions = 0;
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

static inline S32 stat_bonus(int depth) {
#if ENABLEBETTERHISTORYFORMULA
	// Approximately verbatim stat bonus formula from Stockfish // Formula from Ethereal, who took it from stockfish. I love chess programming.
    return depth > 13 ? 32 : 16 * depth * depth + 128 * std::max(depth - 1, 0);
#else
	return std::min(16 * depth * depth + 32 * depth + 16, 1200);
#endif
}

#define MAXHISTORYABS 16384LL
void updateHH(SStack* ss, bool side, BitBoard threats, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount);

Score correctStaticEval(Position& pos, const Score eval);

void updateCorrHist(Position& pos, const Score bonus, const Depth depth);