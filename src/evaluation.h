#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [5] = {130, 432, 447, 623, 1369, };
const Score egValues [5] = {153, 437, 452, 773, 1379, };

const Score mgPawnTable [64] = {

      -1,    6,    6,    3,    3,    0,    3,    7 ,
      36,   37,   22,   35,   23,   39,   15,   -3 ,
       3,    1,    7,   11,   24,   12,    3,   -5 ,
      -1,   -3,    0,    4,    6,    1,   -4,   -1 ,
      -9,  -11,   -6,    2,    3,    0,   -7,   -1 ,
      -9,   -7,   -9,  -11,   -9,  -16,    8,    3 ,
     -43,  -43,  -28,  -33,  -27,  -17,    9,  -11 ,
      -4,    2,    1,    1,   -1,   -1,   -8,    3 ,

};
const Score mgKnightTable [64] = {

    -158,  -92,  -59,  -10,   23,  -45,  -53,  -85 ,
      14,   43,   72,  102,   70,  167,   38,   59 ,
       5,   51,   66,   94,  164,  151,   97,   58 ,
      15,   21,   50,   91,   58,   94,   46,   84 ,
      -6,    5,   18,   28,   47,   39,   58,   30 ,
     -43,  -19,   -6,    1,   17,    7,   27,    9 ,
     -79,  -62,  -42,  -28,  -12,  -25,   -6,   -4 ,
     -77,  -35,  -42,  -10,   -3,    0,  -30,  -40 ,

};
const Score mgBishopTable [64] = {

     -17,  -40,  -47,  -83,  -63,  -60,  -13,  -35 ,
      18,   50,   28,   29,   60,   57,   34,    1 ,
       1,   30,   36,   66,   63,  112,   70,   56 ,
      -5,   11,   36,   70,   61,   52,   22,   12 ,
      -4,   -4,    7,   44,   45,    6,   10,   39 ,
       4,   35,   14,   12,    7,   19,   52,   35 ,
      -9,  -20,   22,  -31,   -5,    3,   39,   46 ,
       8,   47,    9,   -4,    0,    4,   22,   24 ,

};
const Score mgRookTable [64] = {

      20,    8,    4,    5,   24,   46,   51,   71 ,
      49,   45,   54,   87,   67,  110,   83,  115 ,
       0,   35,   30,   37,   97,  102,  133,  112 ,
      -3,   11,    8,   14,   17,   28,   49,   60 ,
     -25,  -26,  -22,  -17,  -17,  -30,   26,   24 ,
     -30,  -23,  -28,  -32,  -23,  -16,   58,   27 ,
     -65,  -58,  -31,  -39,  -23,  -23,   26,  -13 ,
     -12,   -8,   -8,    0,   12,   12,   16,   -5 ,

};
const Score mgQueenTable [64] = {

     -20,  -17,   14,   44,   48,   68,   73,   35 ,
      42,    2,   11,   27,   38,   82,   52,  115 ,
      17,   14,    6,   30,   51,  133,  118,  130 ,
       4,   -2,    6,    3,    5,   37,   46,   56 ,
       0,   -9,   -9,   -3,    4,    2,   24,   48 ,
       9,   14,    0,  -13,  -15,    8,   45,   49 ,
     -18,  -23,    2,   -5,   -1,   -2,   28,   54 ,
       2,   19,   21,   23,   17,   -3,   17,   12 ,

};
const Score mgKingTable [64] = {

      17,   43,   41,    5,   17,   45,   81,   46 ,
      56,  108,   63,   97,   83,  105,  137,  100 ,
      23,  107,   53,   38,   71,  130,  125,   89 ,
      31,   41,   12,  -33,  -26,   19,   43,    3 ,
      19,   20,  -24,  -68,  -61,  -20,  -19,  -50 ,
      49,   77,   -1,  -25,  -24,  -20,   44,   24 ,
     130,   67,   68,   16,   21,   33,  128,  145 ,
     152,  204,  171,   31,  121,   74,  186,  175 ,

};

const Score egPawnTable [64] = {

      -1,   -8,   -4,   -4,   -2,   -1,   -3,   -6 ,
      53,   50,   46,   40,   43,   39,   44,   54 ,
      30,   29,   25,   17,   16,   19,   23,   25 ,
      16,   14,    3,   -4,   -6,    0,    6,    6 ,
       6,    6,   -6,  -11,   -9,   -7,   -1,   -4 ,
     -21,  -24,  -26,  -22,  -18,  -16,  -24,  -28 ,
     -41,  -44,  -39,  -33,  -35,  -37,  -48,  -56 ,
      -1,   -7,   -4,   -5,   -1,   -2,    2,   -5 ,

};
const Score egKnightTable [64] = {

     -61,  -19,    5,  -10,   -6,  -36,  -27,  -84 ,
      52,   61,   43,   37,   22,    9,   43,   19 ,
      33,   33,   61,   50,   23,   14,    7,    6 ,
      23,   28,   51,   49,   49,   42,   26,    4 ,
      20,   14,   43,   44,   50,   32,   10,   12 ,
     -27,  -21,  -12,   18,   14,  -11,  -23,  -19 ,
     -50,  -43,  -38,  -28,  -36,  -37,  -50,  -40 ,
     -14,  -10,   -1,   10,   10,   -7,   -4,    6 ,

};
const Score egBishopTable [64] = {

      19,   26,   21,   26,   20,    7,   11,    2 ,
      57,   62,   60,   49,   35,   44,   55,   49 ,
      56,   47,   48,   25,   24,   32,   33,   40 ,
      28,   32,   26,   41,   27,   29,   21,   22 ,
       8,   21,   33,   31,   28,   24,   15,  -10 ,
     -15,   -4,    2,   13,   24,    8,  -16,  -19 ,
     -33,  -47,  -51,  -15,  -23,  -30,  -33,  -57 ,
       3,   15,   16,   14,   16,   28,    2,  -23 ,

};
const Score egRookTable [64] = {

      59,   61,   71,   61,   51,   54,   50,   43 ,
     109,  124,  117,   93,   95,   86,   93,   82 ,
      88,   85,   86,   71,   53,   48,   50,   41 ,
      69,   61,   70,   59,   42,   43,   41,   33 ,
      49,   48,   46,   41,   42,   44,   25,   20 ,
      17,   12,    8,   15,    7,    3,  -30,  -20 ,
     -16,  -14,  -13,   -9,  -23,  -26,  -43,  -41 ,
      35,   25,   33,   20,   11,   22,   12,   18 ,

};
const Score egQueenTable [64] = {

      54,   64,   84,   77,   85,   73,   38,   54 ,
     106,  148,  161,  166,  182,  144,  133,  130 ,
      89,   96,  143,  144,  151,  131,   96,  102 ,
      73,   91,  101,  130,  149,  132,  131,  115 ,
      51,   80,   83,  115,  112,  110,   94,   88 ,
       4,   31,   47,   62,   75,   69,   41,   25 ,
     -28,  -25,  -18,    0,    1,  -21,  -48,  -63 ,
       7,    5,    8,   27,   18,   15,    1,   -1 ,

};
const Score egKingTable [64] = {

     -41,   15,   27,   56,   48,   61,   57,  -38 ,
      96,  143,  140,  128,  146,  160,  155,  119 ,
      88,  129,  145,  148,  156,  151,  148,  100 ,
      56,  102,  130,  140,  141,  138,  117,   78 ,
      31,   76,  109,  130,  131,  112,   92,   65 ,
       1,   34,   63,   86,   87,   77,   47,   26 ,
     -43,   -5,   12,   34,   38,   27,   -5,  -47 ,
     -43,  -24,    3,   34,   10,   28,  -15,  -63 ,

};

extern Score* mgPestoTables[6];

extern Score* egPestoTables[6];

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };
extern Score mgTables[12][64];
extern Score egTables[12][64];

constexpr Score knightMobMg [9] = {-45, -10, 12, 26, 40, 50, 62, 75, 84, };
constexpr Score bishopMobMg [14] = {-61, -14, 7, 19, 39, 50, 59, 64, 67, 72, 84, 92, 96, 81, };
constexpr Score rookMobMg [15] = {-10, -8, -3, 0, 6, 6, 8, 14, 20, 35, 37, 41, 46, 57, 47, };
constexpr Score queenMobMg [28] = {-20, -109, -46, -16, 2, 7, 14, 20, 27, 34, 41, 48, 52, 59, 62, 69, 71, 72, 78, 96, 101, 125, 136, 140, 151, 141, 89, 73, };

constexpr Score knightMobEg [9] = {-76, -5, 31, 45, 57, 73, 71, 68, 52, };
constexpr Score bishopMobEg [14] = {-48, -32, -1, 25, 40, 56, 66, 70, 76, 76, 69, 67, 63, 56, };
constexpr Score rookMobEg [15] = {-90, 19, 41, 61, 70, 83, 91, 92, 98, 103, 108, 112, 117, 110, 114, };
constexpr Score queenMobEg [28] = {-12, -78, -69, -51, -19, 9, 48, 78, 98, 107, 118, 128, 140, 149, 158, 159, 175, 181, 184, 179, 184, 170, 165, 158, 156, 145, 116, 105, };

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