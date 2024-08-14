#include "history.h"
#include "types.h"
#include "BBmacros.h"
#include "move.h"

// history table
S32 historyTable[2][NUM_SQUARES * NUM_SQUARES];

// capture history table
S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][6];

// counter move table
Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

void updateHistoryBonus(S32* current, Depth depth, bool isBonus) {
    const S32 delta = isBonus ? stat_bonus(depth) : -stat_bonus(depth);
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateHistoryMove(bool side, Move move, Depth depth, S32 delta) {
    S32 *current = &historyTable[side][indexFromTo(moveSource(move),moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateCaptureHistory(Move move, Depth depth, S32 delta) {
    Piece captured = moveCapture(move);
    S32 *current = &captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][captured == NOPIECE ? P : captured % 6]; // account for promotion
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateHistoryScore(S32* current, S32 score, bool isBonus) {
    *current += score - *current * abs(score) / MAXHISTORYABS;
}

void updateHH(bool side, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount) {
    const S32 delta = stat_bonus(depth);
    if (okToReduce(bestMove)) {
        // If bestMove is not noisy, we reduce the bonus of all other moves and increase the bonus of the bestMove
<<<<<<< Updated upstream
        updateHistoryMove(side, bestMove, depth, delta);
        for (int i = 0; i < quietsCount; i++) {
            if (quietMoves[i] == bestMove) continue;
            updateHistoryMove(side, quietMoves[i], depth, -delta);
=======
        updateHistoryMove(side, bestMove, delta);
        // updateContHist(ss, bestMove, delta);
        for (int i = 0; i < quietsCount; i++) {
            if (quietMoves[i] == bestMove) continue;
            updateHistoryMove(side, quietMoves[i], -delta);
            // updateContHist(ss, quietMoves[i], -delta);
>>>>>>> Stashed changes
        }
    }
    else {
        // If bestMove is noisy, we only reduce the bonus of all other noisy moves
        updateCaptureHistory(bestMove, depth, delta);
    }
    for (int i = 0; i < noisyCount; i++) {
        if (noisyMoves[i] == bestMove) continue;
        updateCaptureHistory(noisyMoves[i], depth, -delta);
    }
}