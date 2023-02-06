#pragma once
#include "BBmacros.h"
#include "types.h"
#include "constants.h"

// history table
extern S32 historyTable[2][64][64];
extern S32 pieceToHistoryTable[12][64];		// Indexed by Piece - To
extern S32 pieceFromHistoryTable[12][64];	// Indexed by Piece - From

// killer table
extern Move killerTable[2][maxPly];

// counter move table
extern Move counterMoveTable[NUM_PIECES][NUM_SQUARES];

static inline S32 stat_bonus(int depth) {
#if ENABLEBETTERHISTORYFORMULA
	// Approximately verbatim stat bonus formula from Stockfish // Formula from Ethereal, who took it from stockfish. I love chess programming.
    return depth > 13 ? 32 : 16 * depth * depth + 128 * std::max(depth - 1, 0);
#else
	return depth * depth;
#endif
}

#define MAXHISTORYABS 16384
void updateHistoryBonus(S32* current, Depth depth, bool isBonus) {
    const S32 delta = isBonus ? stat_bonus(depth) : -stat_bonus(depth);
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateHistoryScore(S32* current, S32 score, bool isBonus) {
    *current += score - *current * abs(score) / MAXHISTORYABS;
}
	