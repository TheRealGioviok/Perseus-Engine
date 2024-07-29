#pragma once
#include "BBmacros.h"
#include "types.h"
#include "constants.h"

inline S32 indexFromTo(Square from, Square to) {
	return from * 64 + to;
}

inline S32 indexPieceTo(Piece piece, Square to) {
	return piece * 64 + to;
}

// history table
extern S32 historyTable[2][NUM_SQUARES * NUM_SQUARES];

// counter move table
extern Move counterMoveTable[NUM_PIECES * NUM_SQUARES];

static inline S32 stat_bonus(int depth) {
#if ENABLEBETTERHISTORYFORMULA
	// Approximately verbatim stat bonus formula from Stockfish // Formula from Ethereal, who took it from stockfish. I love chess programming.
    return depth > 13 ? 32 : 16 * depth * depth + 128 * std::max(depth - 1, 0);
#else
	return std::min(16 * depth * depth, 1200);
#endif
}

#define MAXHISTORYABS 16384ULL
void updateHistoryBonus(S32 *current, Depth depth, bool isBonus);
void updateHistoryScore(S32 *current, S32 score, bool isBonus);
void updateHH(bool side, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount);