#pragma once
#include "BBmacros.h"
#include "types.h"
#include "constants.h"
#include "Position.h"

// Forward declaration time
struct Position;

// history table
extern S32 historyTable[2][NUM_SQUARES * NUM_SQUARES][4];

// capture history table
extern S32 captureHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES / 2][4]; // The color of the captured piece is always the opposite of the color of the moving piece

// counter move table
extern Move counterMoveTable[NUM_SQUARES * NUM_SQUARES];

// Continuation History table
extern S32 continuationHistoryTable[NUM_PIECES * NUM_SQUARES][NUM_PIECES * NUM_SQUARES];

constexpr S32 CORRHISTSIZE = 16384;
constexpr S32 CORRHISTSCALE = 256;
constexpr S32 CORRECTIONGRANULARITY = 1024;

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

static inline S32 statBonus(S32 depth){
    return std::min(maxHistoryBonus(), depth * depth * historyBonusQuadDepth() + depth * historyBonusLinDepth() + historyBonusOffset());
}

static inline S32 statMalus(S32 depth){
    return std::min(maxHistoryMalus(), depth * depth * historyMalusQuadDepth() + depth * historyMalusLinDepth() + historyMalusOffset());
}

#define MAXHISTORYABS 16384LL
void updateHH(SStack* ss, bool side, BitBoard threats, Depth depth, Move bestMove, Move *quietMoves, U16 quietsCount, Move *noisyMoves, U16 noisyCount);

Score correctStaticEval(Position& pos, const Score eval);

void updateCorrHist(Position& pos, const Score bonus, const Depth depth);

static inline void updateHistoryMove(const bool side, const BitBoard threats, const Move move, const S32 delta) {
    S32 *current = &historyTable[side][indexFromTo(moveSource(move), moveTarget(move))][getThreatsIndexing(threats, move)];
    *current += delta - *current * abs(delta) / MAXHISTORYABS;
}

static inline void updateCaptureHistory(Move move, const BitBoard threats, S32 delta) {
    Piece captured = moveCapture(move);
    S32 *current = &captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][captured == NOPIECE ? P : captured % 6][getThreatsIndexing(threats, move)]; // account for promotion
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