#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {125, 464, 503, 658, 1443, 0};
const Score mgPawnTable [64] = {
      0,   0,   0,   0,   0,   0,   0,   0 ,
     47,  -2,  46,  77,  51,  33, -88, -64 ,
    -10,  21,  40,  52,  61,  90,  43,  -8 ,
    -23, -22,   3,  12,  24,  17, -21, -20 ,
    -26, -29, -17,   0,  -2,  -2, -23, -23 ,
    -27, -19, -22,  -5,   6, -10,  -6, -18 ,
    -26, -20, -19, -23, -13,  -8,   0, -31 ,
      0,   0,   0,   0,   0,   0,   0,   0 ,
};
const Score mgKnightTable [64] = {
    -199, -157, -135,  -29,   -2, -118, -185, -158 ,
     -14,   -8,   47,  112,   86,  102,  -37,    3 ,
       3,   51,   56,   70,  109,   97,   70,   16 ,
      12,   47,   56,   64,   61,   89,   55,   33 ,
      11,   34,   24,   36,   44,   37,   57,   19 ,
     -35,  -11,    1,    9,   15,    7,    6,  -16 ,
     -37,  -52,  -26,   -9,   -4,  -10,  -29,   -8 ,
     -55,  -36,  -13,  -21,  -19,  -14,  -27,  -63 ,
};
const Score mgBishopTable [64] = {

     -51,  -93, -124, -141, -125, -165,  -60,  -71 ,
     -37,    0,   -4,  -15,    2,    5,   -5,  -35 ,
       5,   44,   32,   31,   43,   33,   49,   34 ,
      12,   17,   27,   52,   55,   36,   25,   11 ,
      -2,   -5,   14,   26,   40,    8,    7,    8 ,
       0,   19,   13,   21,   21,   19,   32,   23 ,
      27,   12,   16,   -1,    2,    9,   32,   25 ,
      24,   17,   -5,   -3,   -8,   -7,    2,   24 ,

};
const Score mgRookTable [64] = {

      27,  15,   9,   9,  32,  21,  35,  65 ,
     -2,  -6,  31,  42,  51,  52,  21,  23 ,
    -16,  42,  28,  48,  77,  49, 100,  30 ,
    -10,  19,  20,  29,  23,  28,  33,  27 ,
    -42, -30, -23, -39, -34, -26,   5, -28 ,
    -61, -17, -33, -42, -37, -26,  10, -34 ,
    -96, -42, -35, -28, -28, -20,   5, -71 ,
    -42, -21, -21, -19, -20, -13, -15, -24 ,

};
const Score mgQueenTable [64] = {

      31,    9,   -7,   36,   17,    5,   17,   17 ,
      -7,  -67,  -15,  -51,  -42,   -6,  -63,  -12 ,
      19,   14,  -11,  -14,    4,   20,   73,   66 ,
       9,   25,   21,    5,    5,    8,   39,   45 ,
       7,    9,    2,   -6,    7,   -4,   22,   25 ,
       5,   19,    9,    6,    4,    7,   32,   24 ,
      -4,    8,   23,   20,   18,   16,   17,    2 ,
      -6,   21,   22,   25,   20,   28,   26,    2 ,

};
const Score mgKingTable [64] = {

      71,  103,   95,   10,   83,  107,  174,  129 ,
     -93,   33,   22,  117,   49,  100,   53,   63 ,
     -71,   94,   71,   17,   37,  123,   86,    8 ,
     -59,    2,   -6,  -64,  -32,   14,    6,  -92 ,
     -74,  -17,  -21,  -97, -102,  -47,  -35, -109 ,
     -67,  -14,  -40, -106,  -88,  -79,  -44,  -72 ,
      38,    1,  -42, -100,  -94,  -71,   11,   34 ,
      32,   46,    6,  -61,   -4,  -54,   44,   49 ,

};
const Score knightMobMg [9] = {-46, -30, -15,  -7,   2,   6,  21,  28,  44,};
const Score bishopMobMg [14] = {-115, -64, -38, -30, -17, -10,  -7,  -5,  -4,   4,  30,  66,  69, 119,};
const Score rookMobMg [15] = {-30, -38, -26, -24, -17, -20, -17, -10,  -4,   4,  11,  13,  23,  42,  96,};
const Score queenMobMg [28] = {-177, -124,  -68,  -56,  -41,  -40,  -29,  -22,  -19,  -15,   -7,   -5,    4,    9,   17,   16,   16,    9,   21,   32,   23,   37,   39,   58,   63,   75,   59,  116,};

const Score egValues [6] = {188, 563, 605, 1043, 1920, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
     76,  78,  50, -14, -32,  -2,  82,  86 ,
     53,  41,   7, -41, -35, -22,  25,  46 ,
     14,   4, -17, -32, -35, -26,   8,  11 ,
     -1,  -9, -24, -31, -36, -25, -20, -10 ,
    -15, -12,  -9, -17, -12, -12, -20, -30 ,
     -6,  -3,   2,   4,  -8,   5, -10, -15 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -69, -13,  26,   8, -18,  20,   1, -111 ,
    -48,   1,  -3,   4,  12, -12, -16, -50 ,
    -28,  -3,  37,  35,  15,  38, -23,  -8 ,
      6,  20,  44,  53,  52,  39,  29,   3 ,
      7,   6,  38,  49,  51,  38,  11,   2 ,
    -36,  -8,   9,  33,  31,  22, -11, -23 ,
     -6,  -2, -14,   6,   4, -15, -11, -16 ,
    -25, -19, -21, -15,  -5, -22, -19, -67 ,

};
const Score egBishopTable [64] = {

      12, 19, 22, 18, 24, 22,  9,  6 ,
     0, 11, 17, -4, -12,  4, 12, -3 ,
     2, -2, 15,  5, 11, 14,  9,  4 ,
     0,  3, 10, 24, 17,  8, -3, -7 ,
    -19, -14, 20, 19, 17, 11, -6, -15 ,
    -30, -8,  8,  7,  5,  2, -16, -18 ,
    -28, -37, -14,  0, -2, -8, -19, -26 ,
    -21, -5,  9, -14, -14,  4, -3, -34 ,

};
const Score egRookTable [64] = {

      28, 39, 40, 41, 38, 37, 36, 21 ,
    28, 41, 38, 28, 27, 23, 31, 24 ,
    26,  4, 20,  6, -3,  4, -12, 11 ,
    21,  8, 18,  1,  5,  2,  3,  6 ,
     1,  2,  0,  0,  9, 12,  2, -15 ,
    -24, -13, -3, -13, -16, -11, -29, -56 ,
    -26, -28, -13, -14, -28, -42, -53, -33 ,
    -33, -21, -18, -34, -35, -21, -23, -47 ,

};
const Score egQueenTable [64] = {

      -7,  43,  56,  73,  77,  58,  59,  48 ,
      7,  47,  96, 113, 142,  97,  89,  24 ,
      5,   5,  72,  89,  75,  84,  14,  11 ,
     -3,  26,  55,  84, 103,  71,  49,  17 ,
    -31,   6,  20,  58,  35,  36,   1,  -1 ,
    -65, -45, -18, -25, -35, -13, -46, -74 ,
    -88, -109, -92, -76, -77, -93, -101, -94 ,
    -97, -115, -112, -82, -102, -122, -117, -118 ,

};
const Score egKingTable [64] = {

    -197,  -98,  -60,   -4,  -30,  -28,  -53, -195 ,
     -48,   38,   31,   -4,   22,   35,   54,  -20 ,
      18,   57,   61,   50,   51,   58,   72,   24 ,
      25,   38,   63,   82,   79,   66,   47,   24 ,
      -8,   13,   59,   73,   74,   61,   33,  -12 ,
     -16,    2,   25,   53,   52,   42,   13,  -17 ,
     -42,   -9,    2,   21,   22,   23,  -16,  -40 ,
    -110,  -71,  -35,  -50,  -66,  -30,  -66, -131 ,

};
const Score knightMobEg [9] = {-119,  -48,   -8,   16,   27,   44,   41,   34,   16, };
const Score bishopMobEg [14] = {-173,  -96,  -36,   -7,   11,   23,   36,   45,   53,   48,   45,   29,   31,   -3,};
const Score rookMobEg [15] = {-151,  -99,  -36,  -34,  -10,   18,   26,   26,   30,   40,   42,   44,   51,   31,   16,};
const Score queenMobEg [28] = {-376, -378, -293, -208,  -92,  -45,    8,   13,   34,   44,   55,   67,   69,   68,   74,   79,   87,   85,   91,   83,   77,   78,   73,   56,   74,   54,   61,   66, };

extern Score* mgPestoTables[6];

extern Score* egPestoTables[6];

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };
extern Score mgTables[12][64];
extern Score egTables[12][64];
/**
* @brief The initTables function initializes the material - positional tables
* @note This function is called once at the beginning of the program
*/
void initTables();

/**
 * @brief The pestoEval function evaluates the material and positional scores
 * @param pos The position to evaluate
 * @return The material and positional scores
 */
Score pestoEval(Position* pos);

/** 
 * @brief The initTropism function initializes the tropism tables
 * @note This is called at the start of the program 
 */
void initTropism();

/**
 * @brief The convertToFeatures function converts a set of positions to a set of features
 * @param filename The filename of the epd file
 * @param output The output file to write the features to
 */
void convertToFeatures(std::string filename, std::string output);