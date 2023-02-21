#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"

// history table
S32 historyTable[2][64][64];
S32 pieceToHistoryTable[12][64];   // Indexed by Piece - To
S32 pieceFromHistoryTable[12][64]; // Indexed by Piece - From

// killer table
Move killerTable[2][maxPly];

// counter move table
Move counterMoveTable[NUM_PIECES][NUM_SQUARES];

void updateHistoryBonus(S32* current, Depth depth, bool isBonus) {
    const S32 delta = isBonus ? stat_bonus(depth) : -stat_bonus(depth);
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateHistoryScore(S32* current, S32 score, bool isBonus) {
    *current += score - *current * abs(score) / MAXHISTORYABS;
}

void updateHH(bool side, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount){
    for (int i = 0; i < quietsCount; i++) {
        updateHistoryBonus(&historyTable[side][moveSource(quietMoves[i])][moveTarget(quietMoves[i])], depth, quietMoves[i] == bestMove);
    }
}