#include "zobrist.h"
#include "random.h"

// random piece keys
HashKey pieceKeys[12][64];
HashKey pawnKeys[12][64];
// random enPassant keys
HashKey enPassantKeys[65];
// random castling keys
HashKey castleKeys[16];
// random side key
HashKey sideKey;
/**
 * @brief The initHashKeys function initializes the random keys.
 */
void initHashKeys(){
    state = 1804289383;
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 64; j++){
            pieceKeys[i][j] = getRandom64();
            if (i % 6 == 0) pawnKeys[i][j] = pieceKeys[i][j];
            else pawnKeys[i][j] = 0;
        }
    }
    for(int i = 0; i < 64; i++){
        enPassantKeys[i] = getRandom64();
    }
    enPassantKeys[64] = 0; // So that hashing a "no en passant" will return the same hashKey as before the move
    for(int i = 0; i < 16; i++){
        castleKeys[i] = getRandom64();
    }
    sideKey = getRandom64();
}