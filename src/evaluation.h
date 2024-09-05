#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {102, 399, 427, 569, 1148, 0};
const Score mgPawnTable [64] = {

       8,   12,   12,   10,    9,    7,   15,   13 ,
       3,    6,    6,    3,    3,    4,   13,    2 ,
      -4,    2,    2,    1,    3,    1,    4,   -3 ,
      -2,   -7,   -3,    3,    5,   -2,   -7,   -4 ,
      -1,   -1,    4,   10,    8,    8,    0,   -1 ,
       1,   -3,   -4,   -3,    2,   -4,   -1,    4 ,
      -4,   -6,   -8,   -6,   -7,   -3,   -8,   -9 ,
      -5,  -10,   -7,   -9,   -7,   -9,  -18,  -10 ,

};
const Score mgKnightTable [64] = {

    -140,  -99,  -76,  -14,    5,  -77,  -98,  -92 ,
     -22,  -15,   37,   92,   62,   88,   -6,   -3 ,
       2,   48,   52,   67,  103,   76,   70,   12 ,
      14,   27,   44,   56,   51,   65,   44,   29 ,
       4,   23,   18,   25,   36,   28,   48,   24 ,
     -39,  -17,   -6,   -1,    5,    4,    0,  -13 ,
     -38,  -53,  -32,  -11,   -7,  -23,  -38,  -16 ,
     -61,  -45,  -30,  -33,  -23,  -27,  -43,  -59 ,

};
const Score mgBishopTable [64] = {

     -38, -62, -81, -102, -85, -108, -35, -39 ,
    -50,  -6,  -2, -17,  12,  10,   5, -35 ,
      1,  25,  26,  31,  36,  44,  42,  25 ,
      0,  16,  23,  45,  47,  29,  25,   5 ,
     -3,   1,  11,  30,  36,  13,  10,   4 ,
      1,  18,  11,  18,  16,  16,  20,  14 ,
     15,   4,   6,  -7,  -3,   4,  14,  19 ,
     14,   5,  -6, -15, -18,  -9,  -9,  11 ,

};
const Score mgRookTable [64] = {

     21,   8,  -6,  -5,   3,   5,  33,  45 ,
      3,   5,  25,  39,  37,  47,  28,  23 ,
     -8,  47,  27,  47,  67,  59,  81,  31 ,
     -9,  13,  18,  31,  27,  28,  36,  24 ,
    -33, -14, -18, -20, -24, -18,   3, -11 ,
    -49, -14, -28, -32, -25, -24,  14, -18 ,
    -80, -42, -30, -25, -23, -13, -13, -62 ,
    -39, -35, -28, -22, -21, -16, -25, -25 ,

};
const Score mgQueenTable [64] = {

      17,   3,  14,  32,  24,  32,  21,  14 ,
    -22, -61, -24, -39, -25,   9, -45, -20 ,
      0,   5,  -9,  -2,   2,  35,  50,  54 ,
     -3,   4,   0, -12, -11,   7,  22,  30 ,
     -9,   2, -14, -12,  -4,  -7,  11,  15 ,
     -9,  -2,  -6, -12,  -7,  -2,  12,   8 ,
    -16,  -7,   2,   3,  -1,   9,  -7, -15 ,
    -15,  -4,  -1,   3,   1,   0,  -2, -12 ,

};
const Score mgKingTable [64] = {

     -16,  23,  19,   0,  21,  29,  68,   3 ,
    -44,  31,  19,  64,  40,  69,  50,  33 ,
    -30,  76,  59,  34,  44, 101,  76,  12 ,
    -35,  21,  17, -32, -17,  20,  21, -50 ,
    -47,  16,  -8, -61, -59, -18, -10, -75 ,
    -32, -14, -36, -74, -70, -58, -37, -46 ,
     46,  -2, -27, -79, -70, -54,  -1,  38 ,
     28,  38,  10, -52,  -1, -45,  40,  43 ,

};
constexpr Score knightMobMg [9] = {-53, -35, -17,  -6,   2,  11,  20,  32,  42 };
constexpr Score bishopMobMg [14] = {-102, -60, -36, -25, -12,  -4,   1,   3,   7,  13,  25,  47,  62,  77 };
constexpr Score rookMobMg [15] = {-34, -39, -27, -23, -20, -20, -16,  -8,  -2,   9,  13,  22,  24,  42,  73};
constexpr Score queenMobMg [28] = {-162, -124, -79, -57, -53, -46, -36, -30, -23, -15,  -8,  -1,   3,   6,   8,  10,  11,  10,  10,  22,  26,  39,  53,  71,  89, 110,  84,  89 };

const Score egValues [6] = {154, 461, 490, 856, 1499, 0};
const Score egPawnTable [64] = {

      -4,   -7,   -4,   -2,   -4,   -1,   -9,  -10 ,
      -9,   -7,    0,    4,    3,    1,   -7,  -12 ,
      -4,    0,    1,    2,    3,    3,    0,   -6 ,
       4,    4,    0,   -6,   -7,   -2,    2,    1 ,
       8,    8,   -2,   -9,  -10,   -5,    5,    5 ,
       5,    1,   -2,   -6,   -6,   -3,    0,    2 ,
      10,    7,    7,    1,    2,    6,    5,    8 ,
       2,    4,    2,    1,    4,    1,    9,    8 ,

};
const Score egKnightTable [64] = {

     -65, -16,  16,  -1,  -5,  10, -10, -94 ,
    -31,  -5,  -5,   1,   8, -14, -13, -41 ,
    -15,  -5,  35,  35,  24,  30, -14, -16 ,
     -6,  12,  39,  48,  49,  32,  16,  -5 ,
     12,  11,  33,  39,  40,  30,  10,   6 ,
    -15,  -8,  12,  24,  24,  10,  -8, -21 ,
     -4,   4, -13,  -1,  -2, -16,  -4,  -1 ,
    -40, -17, -25, -13,  -6, -19, -10, -42 ,

};
const Score egBishopTable [64] = {

      12, 13, 12, 19, 19, 10,  1, -2 ,
    -7,  3,  6,  5, -5,  2,  1, -14 ,
     4,  9,  9,  1,  3,  8,  4, -3 ,
    -1,  2,  3, 13, 11,  4, -1,  0 ,
    -8, -2, 12,  8,  8,  7, -3, -10 ,
    -8,  2,  4,  2,  3,  0, -5, -6 ,
    -11, -19, -17, -5, -3, -12, -14, -16 ,
    -18, -7,  0, -15, -7,  2, -2, -18 ,

};
const Score egRookTable [64] = {

      26, 29, 39, 37, 34, 37, 24, 17 ,
    16, 28, 30, 28, 29, 20, 22, 10 ,
    17,  5, 20, 13,  7, 11, -6,  4 ,
    12,  8, 16,  8,  9,  7,  3,  2 ,
     1, 10,  7,  3,  4,  7,  6, -5 ,
    -16, -15, -18, -22, -24, -23, -26, -32 ,
    -15, -19, -22, -30, -33, -36, -37, -28 ,
    -19, -19, -20, -28, -29, -23, -20, -26 ,

};
const Score egQueenTable [64] = {

     -12,  17,  34,  47,  57,  39,  17,   6 ,
      2,  46,  62,  95, 109,  65,  58,  23 ,
      6,  10,  61,  80,  76,  78,  15,  17 ,
      9,  32,  41,  72,  85,  61,  50,  24 ,
    -10,  13,  21,  48,  38,  37,  11,   6 ,
    -40, -33, -16, -21, -23, -10, -31, -42 ,
    -59, -76, -83, -66, -61, -89, -79, -69 ,
    -85, -91, -96, -74, -88, -113, -100, -100 ,

};
const Score egKingTable [64] = {

    -128,  -61,  -44,  -18,  -36,  -23,  -28, -135 ,
     -29,   33,   25,    3,   13,   26,   43,  -33 ,
       6,   49,   48,   45,   45,   46,   53,    6 ,
       1,   35,   53,   60,   58,   53,   40,    3 ,
     -17,   17,   39,   56,   56,   40,   25,   -7 ,
     -14,    2,   21,   37,   34,   23,    6,  -10 ,
     -30,   -5,    4,   15,   14,   10,   -8,  -34 ,
    -100,  -56,  -34,  -38,  -51,  -27,  -53, -107 ,

};
const Score knightMobEg [9] = {-99, -41,  -7,  13,  23,  37,  35,  28,   8  };
const Score bishopMobEg [14] = {-166,  -98,  -46,  -16,    2,   21,   33,   38,   47,   47,   44,   36,   40,   14,};
const Score rookMobEg [15] = {-138, -90, -45, -23,  -3,  13,  22,  22,  28,  34,  40,  41,  44,  33,  20,};
const Score queenMobEg [28] = {-342, -346, -289, -197, -93, -41,  -2,  22,  41,  49,  59,  65,  73,  81,  86,  93,  99, 104, 106, 100,  99,  89,  75,  62,  34,  18, -27, -23 };

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