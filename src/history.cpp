#include "BBmacros.h"
#include "types.h"
#include "move.h"
#include "history.h"
#include "Game.h"

// history table
S32 historyTable[2][NUM_SQUARES * NUM_SQUARES];

// Similarity History table
S32 pawnStructuredHistoryTable[2][2 * NUM_SQUARES * NUM_SQUARES];
//S32 pieceStructuredHistoryTable[2][2][NUM_SQUARES * NUM_SQUARES];
BitBoard pawnIndices[2];
//BitBoard structureIndices[2];

// capture history table
S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][6];

// counter move table
Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

void updateHistoryMove(bool side, Move move, S32 hhDelta, S32 p1delta, S32 p2delta) {
    S32 *current = &historyTable[side][indexFromTo(moveSource(move),moveTarget(move))];
    *current += hhDelta - *current * abs(hhDelta) / MAXSMALLHISTORYABS;
    current = &pawnStructuredHistoryTable[0][indexSideFromTo(side, moveSource(move),moveTarget(move))];
    *current += p1delta - *current * abs(p1delta) / MAXSMALLHISTORYABS;
    current = &pawnStructuredHistoryTable[1][indexSideFromTo(side, moveSource(move),moveTarget(move))];
    *current += p2delta - *current * abs(p2delta) / MAXSMALLHISTORYABS;
}

void updateCaptureHistory(Move move, S32 delta) {
    Piece captured = moveCapture(move);
    S32 *current = &captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][captured == NOPIECE ? P : captured % 6]; // account for promotion
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateContHistOffset(SStack* ss, const Move move, const S32 delta, const S32 offset){
    S32 *current = &(ss - offset)->contHistEntry[indexPieceTo(movePiece(move), moveTarget(move))];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

void updateContHist(SStack* ss, const Move move, const S32 delta){
    updateContHistOffset(ss, move, delta, 1);
    updateContHistOffset(ss, move, delta, 2);
}

void updateHH(SStack* ss, bool side, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount, BitBoard pawnSimMask) {
    const S32 delta = stat_bonus<true>(depth);
    const S32 deltaI = stat_bonus<true>(depth);
    const S32 pawnSimIndices[2] = { editDist(pawnIndices[0], pawnSimMask) << 8, editDist(pawnIndices[1], pawnSimMask) << 8}; // We might want to think of a better dist function
    const S32 err = pawnSimIndices[0] + pawnSimIndices[1] + 1; // For now, dumb way to avoid 0 div, have to think of something better
    const S32 pawnDeltas[2] = {deltaI * pawnSimIndices[1] / err, deltaI * pawnSimIndices[0] / err}; // Simplification of err - indices = otherindices
    if (okToReduce(bestMove)) {
        // If bestMove is not noisy, we reduce the bonus of all other moves and increase the bonus of the bestMove
        updateHistoryMove(side, bestMove, delta, pawnDeltas[0], pawnDeltas[1]);
        updateContHist(ss, bestMove, delta);
        for (int i = 0; i < quietsCount; i++) {
            if (quietMoves[i] == bestMove) continue;
            updateHistoryMove(side, quietMoves[i], -delta, -pawnDeltas[0], -pawnDeltas[1]);
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