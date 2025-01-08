#include "zobrist.h"
#include "random.h"

// random piece keys
HashKey pieceKeysTable[12][64];
// random enPassant keys
HashKey enPassantKeysTable[65];
// random castling keys
HashKey castleKeysTable[16];
// random side key
HashKey sideKey;
/**
 * @brief The initHashKeys function initializes the random keys.
 */
void initHashKeys(){
    state = 1804289383;
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 64; j++){
            pieceKeysTable[i][j] = getRandom64();
        }
    }
    for(int i = 0; i < 64; i++){
        enPassantKeysTable[i] = getRandom64();
    }
    enPassantKeysTable[64] = 0; // So that hashing a "no en passant" will return the same hashKey as before the move
    for(int i = 0; i < 16; i++){
        castleKeysTable[i] = getRandom64();
    }
    sideKey = getRandom64();
}