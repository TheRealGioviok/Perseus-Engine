#include "tables.h"
#include "evaluation.h"
#include "types.h"
#include "random.h"
#include "BBmacros.h"
#include "movegen.h"
#include "zobrist.h"
#include "tt.h"
#include <cmath>

// bishop attack masks
BitBoard bishopMasks[64];
// rook attack masks
BitBoard rookMasks[64];
// bishop attacks table
BitBoard bishopAttacks[64][512];
// rook attacks table
BitBoard rookAttacks[64][4096];

// tropism
int distBonus[64][64];
int qkdist[64][64];
int rkdist[64][64];
int nkdist[64][64];
int bkdist[64][64];
int kbdist[64][64];
// mhDist
U8 manhattanDistance[64][64];
// cbyDist
U8 chebyshevDistance[64][64];
// centerDistance
U8 centerDistance[64];
// squaresBetween
BitBoard squaresBetween[64][64]; // TODO: squaresBetween doesn't quite work...

// pv table and pv length
Move pvTable[maxPly][maxPly] = { {0} };
Ply pvLen[maxPly] = { 0 };
// Repetition Table
HashKey repetitionTable[512] = { 0ULL };

BitBoard squaresToRight[64];
BitBoard squaresToLeft[64];
BitBoard passedPawnMask[2][64];
BitBoard isolatedPawnMask[64];
BitBoard backwardPawnMask[2][64];
BitBoard phalanx[64];
BitBoard outerRing[64];
BitBoard fiveSquare[64]; // The 5x5 square
BitBoard kingShelter[2][64]; // The king shelter

// The LMR reduction table
Ply reductionTable[2][64][64] = {{0}};
S16 lmpMargin[128][2] = {{0}};
#define RESOLUTION 1000
double lmrDepthValue = 1000;
double lmrMoveValue = 1000;
double lmrA0 = 763;
double lmrC0 = -275;
double lmrA1 = 775;
double lmrC1 = 625;
/**
 * @brief The initLMRTable function initializes the LMR reduction table
 */
void initLMRTable(){
    for (int depth = 0; depth < 64; depth++) {
        for (int move = 0; move < 64; move++) {
            reductionTable[0][depth][move] = std::max(0, 
                int(
                    lmrA0 / RESOLUTION *
                    log(depth * lmrDepthValue / RESOLUTION) *
                    log(move * lmrMoveValue / RESOLUTION) +
                    lmrC0 / RESOLUTION
                )
            );
            reductionTable[1][depth][move] = std::max(0, 
                int(
                    lmrA1 / RESOLUTION *
                    log(depth * lmrDepthValue / RESOLUTION) *
                    log(move * lmrMoveValue / RESOLUTION) +
                    lmrC1 / RESOLUTION
                )
            );
        }
    }
    // Init lmpMargin
    for (int depth = 0; depth < 128; depth++) {
        for (int improving = 0; improving < 2; improving++) {
            lmpMargin[depth][0] = 8 * depth; // 1.5 + 0.5 * std::pow(depth, 2.0); // Not improving
            lmpMargin[depth][1] = 8 * depth; // 3.0 + 1.0 * std::pow(depth, 2.0); // improving
        }
    }
}

/**
 * @brief The initEvalTables function initializes the evaluation tables.
 */
void initEvalTables() {
    //BitBoard squaresToLeft[64];
    for (Square square = a8; square < noSquare; square++) {
        BitBoard res = ranks(rankOf(square));
        for (Square clear = square; clear < noSquare; clear++) clearBit(res, clear);
        squaresToLeft[square] = res;
    }
    //BitBoard squaresToRight[64];
    for (Square square = 0; square < noSquare; square++) {
        BitBoard res = ranks(rankOf(square));
        res ^= (squareBB(square) | squaresToLeft[square]);
        squaresToRight[square] = res;
    }

    //BitBoard isolatedPawnMask[64]
    for (Square square = 0; square < 64; square++) {
        BitBoard res = 0;
        res |= (fileOf(square) >= 1 ? files(fileOf(square) - 1) : 0);
        res |= (fileOf(square) <= 6 ? files(fileOf(square) + 1) : 0);
        //res &= (~squareBB(square));
        isolatedPawnMask[square] = res;
    }

    //BitBoard passedPawnMask[2][64];
    for (Square square = a8; square < noSquare; square++) {

        BitBoard res = squaresAhead(square) | ((fileOf(square) != 7) ? squaresAhead(square + 1) : 0) | ((fileOf(square) != 0)  ? squaresAhead(square - 1) : 0);
        passedPawnMask[WHITE][square] = res;

        res = squaresBehind(square) | ((fileOf(square) != 7) ? squaresBehind(square+1) : 0 ) | ((fileOf(square) != 0) ? squaresBehind(square - 1) : 0 );
        passedPawnMask[BLACK][square] = res;
        
    }

    //BitBoard backwardPawnMask[2][64];
    for (int side = 0; side < 2; side++) {
        for (Square square = 0; square < 64; square++){
            BitBoard res = passedPawnMask[side^1][square];
            res &= notFile(fileOf(square));
            res &= notRank(0);
            res &= notRank(7);
            if (fileOf(square) > 0) res |= squareBB(square - 1);
            if (fileOf(square) < 7) res |= squareBB(square + 1);
            backwardPawnMask[side][square] = res;
        }
    }

    //BitBoard phalanx[64]
    for (Square square = 0; square < 64; square++){
        phalanx[square] = 0;
        if (fileOf(square) > 0) phalanx[square] |= squareBB(square - 1);
        if (fileOf(square) < 7) phalanx[square] |= squareBB(square + 1);
    }

    // mha, cby distance
    for (Square square1 = a8; square1 < noSquare; square1++){
        for (Square square2 = a8; square2 < noSquare; square2++){
            int file1, file2, rank1, rank2;
            int rankDistance, fileDistance;
            file1 = square1 & 7;
            file2 = square2 & 7;
            rank1 = square1 >> 3;
            rank2 = square2 >> 3;
            rankDistance = abs(rank2 - rank1);
            fileDistance = abs(file2 - file1);
            manhattanDistance[square1][square2] = rankDistance + fileDistance;
            chebyshevDistance[square1][square2] = std::max(rankDistance, fileDistance);
        }
        int file, rank;
        file = square1 & 7;
        rank = square1 >> 3;
        file ^= (file - 4) >> 8;
        rank ^= (rank - 4) >> 8;
        centerDistance[square1] = (file + rank) & 7;
    }

    // squaresBetween[64][64]

    for (Square sq1 = a8; sq1 < noSquare; sq1++) {
        int rank = rankOf(sq1);
        int file = fileOf(sq1);
        // We generate the squares on left
        {
            BitBoard between = 0ULL;
            for (int i = file - 1; i >= 0; i--) {
                squaresBetween[sq1][makeSquare(rank,i)] = between;
                between |= squareBB(makeSquare(rank, i));
            }
        }
        // We generate the squares on right
        {
            BitBoard between = 0ULL;
            for (int i = file + 1; i < 8; i++) {
                squaresBetween[sq1][makeSquare(rank,i)] = between;
                between |= squareBB(makeSquare(rank, i));
            }
        }
        // We generate the squares on top
        {
            BitBoard between = 0ULL;
            for (int i = rank - 1; i >= 0; i--) {
                squaresBetween[sq1][makeSquare(i,file)] = between;
                between |= squareBB(makeSquare(i, file));
            }
        }
        // We generate the squares on bottom
        {
            BitBoard between = 0ULL;
            for (int i = rank + 1; i < 8; i++) {
                squaresBetween[sq1][makeSquare(i,file)] = between;
                between |= squareBB(makeSquare(i, file));
            }
        }
        // NW diagonal:
        {
            BitBoard between = 0ULL;
            for (int i = rank - 1, j = file - 1; i >= 0 && j >= 0; i--, j--) {
                squaresBetween[sq1][makeSquare(i,j)] = between;
                between |= squareBB(makeSquare(i, j));
            }
        }
        // NE diagonal:
        {
            BitBoard between = 0ULL;
            for (int i = rank - 1, j = file + 1; i >= 0 && j < 8; i--, j++) {
                squaresBetween[sq1][makeSquare(i,j)] = between;
                between |= squareBB(makeSquare(i, j));
            }
        }
        // SW diagonal:
        {
            BitBoard between = 0ULL;
            for (int i = rank + 1, j = file - 1; i < 8 && j >= 0; i++, j--) {
                squaresBetween[sq1][makeSquare(i,j)] = between;
                between |= squareBB(makeSquare(i, j));
            }
        }
        // SE diagonal:
        {
            BitBoard between = 0ULL;
            for (int i = rank + 1, j = file + 1; i < 8 && j < 8; i++, j++) {
                squaresBetween[sq1][makeSquare(i,j)] = between;
                between |= squareBB(makeSquare(i, j));
            }
        }
		
		// Outer ring
        {
            BitBoard ring = 0ULL;
            for (int i = -1; i < 2; i++){
                for (int j = -1; j < 2; j++){
                    if (rank + i < 0 || rank + i > 7 || file + j < 0 || file + j > 7) continue;
                    ring |= kingAttacks[makeSquare(rank + i, file + j)];
                }
            }
            fiveSquare[sq1] = ring;
            ring &= ~(kingAttacks[sq1] | squareBB(sq1));
			
            outerRing[sq1] = ring;
        }
      
        // King shelter
        {
            BitBoard sq = squareBB(sq1);
            kingShelter[WHITE][sq1] = north(sq) | ne(sq) | nw(sq);
            kingShelter[BLACK][sq1] = south(sq) | se(sq) | sw(sq);
        }
    }
}

void initTropism(){
    int diag_nw[64] = {
   0, 1, 2, 3, 4, 5, 6, 7,
   1, 2, 3, 4, 5, 6, 7, 8,
   2, 3, 4, 5, 6, 7, 8, 9,
   3, 4, 5, 6, 7, 8, 9,10,
   4, 5, 6, 7, 8, 9,10,11,
   5, 6, 7, 8, 9,10,11,12,
   6, 7, 8, 9,10,11,12,13,
   7, 8, 9,10,11,12,13,14
    };

    int diag_ne[64] = {
       7, 6, 5, 4, 3, 2, 1, 0,
       8, 7, 6, 5, 4, 3, 2, 1,
       9, 8, 7, 6, 5, 4, 3, 2,
      10, 9, 8, 7, 6, 5, 4, 3,
      11,10, 9, 8, 7, 6, 5, 4,
      12,11,10, 9, 8, 7, 6, 5,
      13,12,11,10, 9, 8, 7, 6,
      14,13,12,11,10, 9, 8, 7
    };

    int bonus_dia_distance[15] = { 5, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    /* initializes the table of distances between squares */
    int i, j;

    /* basic distance table used to generate separate tables for pieces */
    for (i = 0; i < 64; ++i) {
        for (j = 0; j < 64; ++j) {
            distBonus[i][j] = 14 - (abs(fileOf(i) - fileOf(j)) + abs(rankOf(i) - rankOf(j)));

            qkdist[i][j] = (distBonus[i][j] * 5) / 2;
            rkdist[i][j] = distBonus[i][j] / 2;
            nkdist[i][j] = distBonus[i][j];
            /* bk_dist[i][j] takes into account the numbers of the diagonals */
            bkdist[i][j] = distBonus[i][j] / 2;
            kbdist[i][j] = bonus_dia_distance[abs(diag_ne[i] - diag_ne[j])];
            kbdist[i][j] += bonus_dia_distance[abs(diag_nw[i] - diag_nw[j])];
        }
    }
}
        
    

/**
 * @brief The initAll function initializes the tables.
 * @note THis is called at the beginning of the program.
 */
void initAll(){
    initLMRTable();
    initEvalTables();
    initTropism();
    //initMagic();
    initSliderAttacks(true);
    initSliderAttacks(false);
    initHashKeys();
    initTables();
    initTT();
}