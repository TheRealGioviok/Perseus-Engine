#include "zobrist.h"
#include "random.h"

// random piece keys
HashKey pieceKeysTable[12][64];
HashKey pawnKeysTable[12][64];
HashKey nonPawnKeysTable[12][64];
HashKey minorKeysTable[12][64];
HashKey rookPawnKeysTable[12][64];
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
            switch (i % 6){
                case 0:
                    pawnKeysTable[i][j] = pieceKeysTable[i][j];
                    minorKeysTable[i][j] = 0;
                    nonPawnKeysTable[i][j] = 0;
                    rookPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    break;
                case 1: case 2:
                    pawnKeysTable[i][j] = 0;
                    minorKeysTable[i][j] = pieceKeysTable[i][j];
                    nonPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    rookPawnKeysTable[i][j] = 0;
                    break;
                case 3:
                    pawnKeysTable[i][j] = 0;
                    minorKeysTable[i][j] = 0;
                    nonPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    rookPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    break;
                case 4:
                    pawnKeysTable[i][j] = 0;
                    minorKeysTable[i][j] = 0;
                    nonPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    rookPawnKeysTable[i][j] = 0;
                    break;
                case 5:
                    pawnKeysTable[i][j] = 0;
                    minorKeysTable[i][j] = pieceKeysTable[i][j];
                    nonPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    rookPawnKeysTable[i][j] = pieceKeysTable[i][j];
                    break;
            }
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