#pragma once
#include "types.h"

// random piece keys
extern HashKey pieceKeysTable[12][64];
extern HashKey pawnKeysTable[12][64];
extern HashKey nonPawnKeysTable[12][64];
// random enPassant keys
extern HashKey enPassantKeysTable[65];
// random castling keys
extern HashKey castleKeysTable[16];
// random side key
extern HashKey sideKey;

/**
 * @brief The initHashKeys function initializes the random keys.
 */
void initHashKeys();