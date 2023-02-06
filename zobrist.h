#pragma once
#include "types.h"

// random piece keys
extern HashKey pieceKeys[12][64];
// random enPassant keys
extern HashKey enPassantKeys[65];
// random castling keys
extern HashKey castleKeys[16];
// random side key
extern HashKey sideKey;

/**
 * @brief The initHashKeys function initializes the random keys.
 */
void initHashKeys();