#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"

// history table
S32 historyTable[2][NUM_SQUARES * NUM_SQUARES];
S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];
// counter move table
Move counterMoveTable[NUM_PIECES][NUM_SQUARES];

void updateHistoryBonus(S32* current, Depth depth, bool isBonus) {
    const S32 delta = isBonus ? stat_bonus(depth) : -stat_bonus(depth);
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateHH(bool side, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount){
    for (int i = 0; i < quietsCount; i++) {
        updateHistoryBonus(&historyTable[side][indexFromTo(moveSource(quietMoves[i]),moveTarget(quietMoves[i]))], depth, quietMoves[i] == bestMove);
    }
}

void updateContHist(Depth depth, Move prev, Move bestMove, Move *quietMoves, U16 quietsCount){
    for (int i = 0; i < quietsCount; i++) {
        updateHistoryBonus(&continuationHistoryTable[indexPieceTo(movePiece(prev), moveTarget(prev))][indexPieceTo(movePiece(quietMoves[i]), moveTarget(quietMoves[i]))], depth, quietMoves[i] == bestMove);
    }
}