#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {111, 402, 417, 582, 1116, 0};
const Score mgPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      47,    6,   38,   66,   60,   35,  -63,  -60 ,
      -1,   31,   55,   52,   64,   88,   56,    1 ,
      -5,  -12,    4,   17,   26,   20,  -10,   -7 ,
     -19,  -26,  -10,    7,    6,    3,  -20,  -16 ,
     -16,  -12,  -10,   -2,    8,   -7,    1,  -11 ,
     -18,  -13,  -14,  -14,  -11,   -1,    5,  -20 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score mgKnightTable [64] = {

    -164, -112,  -83,   -6,   16,  -78, -117, -118 ,
      -8,   -9,   46,  109,   73,  101,  -12,    9 ,
      17,   56,   64,   77,  111,   89,   79,   27 ,
      24,   43,   61,   72,   67,   83,   61,   39 ,
      12,   34,   29,   37,   49,   37,   57,   30 ,
     -33,   -8,    3,   13,   17,   13,    8,   -7 ,
     -30,  -40,  -15,   -1,    3,   -8,  -18,   -9 ,
     -55,  -31,  -17,  -19,  -13,  -15,  -23,  -47 ,

};
const Score mgBishopTable [64] = {

     -39,  -68,  -84, -101,  -86, -104,  -43,  -43 ,
     -36,   -3,    2,  -12,   17,   13,   -2,  -25 ,
      15,   35,   30,   35,   41,   47,   50,   36 ,
       9,   30,   36,   53,   57,   40,   37,   13 ,
       7,    8,   17,   37,   43,   18,   15,   11 ,
       9,   26,   19,   26,   25,   25,   28,   24 ,
      24,   17,   17,    2,    7,   13,   34,   25 ,
      24,   16,    6,   -3,   -7,    2,    8,   26 ,

};
const Score mgRookTable [64] = {

      41,   20,    8,   11,   23,   24,   46,   64 ,
      16,   12,   33,   51,   49,   63,   33,   37 ,
       7,   60,   39,   60,   81,   75,  101,   47 ,
       3,   30,   34,   48,   46,   48,   57,   36 ,
     -29,   -7,  -12,  -11,  -14,  -10,   12,   -8 ,
     -44,   -3,  -19,  -21,  -16,  -12,   27,  -13 ,
     -71,  -28,  -16,  -14,  -11,    0,    8,  -52 ,
     -26,  -15,  -10,   -3,   -4,    0,    2,   -8 ,

};
const Score mgQueenTable [64] = {

      74,   52,   62,   83,   79,   81,   66,   65 ,
      36,  -20,   19,   -1,   16,   54,  -17,   31 ,
      60,   61,   40,   46,   53,   80,  107,  111 ,
      51,   62,   56,   39,   40,   61,   79,   83 ,
      43,   51,   36,   39,   46,   42,   60,   64 ,
      42,   53,   48,   42,   47,   51,   67,   59 ,
      37,   51,   59,   57,   54,   63,   57,   37 ,
      40,   56,   58,   60,   58,   58,   64,   46 ,

};
const Score mgKingTable [64] = {

      -3,   38,   35,    3,   29,   39,   78,   11 ,
     -42,   35,   18,   77,   42,   81,   53,   44 ,
     -37,   91,   67,   27,   43,  115,   93,   14 ,
     -51,   25,   15,  -40,  -21,   19,   24,  -69 ,
     -62,    6,  -21,  -76,  -76,  -30,  -25,  -93 ,
     -46,  -23,  -44,  -86,  -81,  -67,  -43,  -60 ,
      45,   -2,  -30,  -93,  -84,  -64,    4,   37 ,
      33,   43,   11,  -53,   -6,  -51,   46,   51 ,

};
const Score knightMobMg [9] = {-25, -10, 4, 13, 20, 28, 36, 47, 57, };
const Score bishopMobMg [14] = {-65, -21, 4, 14, 24, 31, 37, 40, 44, 52, 65, 92, 107, 130, };
const Score rookMobMg [15] = {-16, -21, -9, -6, -4, -5, 0, 6, 10, 21, 25, 35, 39, 57, 95, };
const Score queenMobMg [28] = {-88, -52, -7, 13, 19, 27, 34, 40, 46, 52, 59, 65, 69, 73, 75, 76, 77, 75, 79, 86, 93, 105, 116, 141, 169, 191, 177, 167, };

const Score egValues [6] = {170, 478, 509, 873, 1381, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      70,   76,   44,  -12,  -13,   18,   79,   86 ,
      63,   56,   14,  -25,  -31,   -5,   39,   55 ,
      26,   19,   -2,  -25,  -27,  -14,   12,   16 ,
       4,    1,  -13,  -24,  -25,  -17,   -7,   -5 ,
      -3,   -8,   -5,   -9,   -8,   -5,  -12,  -13 ,
       5,    5,   13,   12,   11,   13,   -2,   -5 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -53,    1,   33,   14,   14,   28,   16,  -81 ,
     -14,   15,   12,   15,   26,    2,    8,  -19 ,
       1,   10,   45,   47,   36,   39,    3,    2 ,
      13,   30,   54,   65,   66,   46,   36,   15 ,
      26,   25,   50,   58,   60,   49,   25,   20 ,
      -5,    6,   32,   49,   48,   32,    8,  -10 ,
       9,   14,    3,   20,   19,    2,    4,    9 ,
     -20,   -7,   -7,    5,   10,    0,   -2,  -29 ,

};
const Score egBishopTable [64] = {

      35,   30,   30,   36,   38,   28,   20,   26 ,
      19,   27,   25,   21,   13,   21,   27,   16 ,
      28,   25,   26,   16,   18,   26,   21,   25 ,
      22,   24,   21,   34,   33,   23,   24,   25 ,
      10,   16,   34,   30,   31,   31,   18,    9 ,
       8,   15,   24,   25,   26,   22,   11,    8 ,
       3,   -6,    3,   18,   19,    8,   -3,   -4 ,
       0,   10,   20,    5,   11,   22,   12,   -7 ,

};
const Score egRookTable [64] = {

      56,   65,   73,   71,   69,   70,   62,   53 ,
      51,   59,   57,   50,   52,   46,   52,   47 ,
      47,   29,   41,   31,   25,   29,   18,   35 ,
      41,   34,   39,   30,   30,   29,   30,   31 ,
      24,   34,   35,   33,   33,   37,   32,   19 ,
       5,   13,   16,   16,   14,   14,    3,  -12 ,
       5,    4,    8,    6,    1,   -4,  -14,   -8 ,
       7,    7,   10,    2,   -1,   10,    1,   -6 ,

};
const Score egQueenTable [64] = {

     124,  162,  182,  196,  204,  188,  168,  156 ,
     140,  188,  201,  239,  255,  208,  210,  171 ,
     143,  137,  197,  216,  212,  219,  146,  162 ,
     151,  171,  173,  212,  227,  200,  194,  170 ,
     121,  147,  160,  188,  180,  181,  149,  143 ,
      87,   98,  125,  122,  120,  134,  106,   88 ,
      69,   54,   50,   75,   81,   48,   50,   61 ,
      53,   44,   44,   64,   52,   31,   32,   31 ,

};
const Score egKingTable [64] = {

    -152,  -69,  -48,  -12,  -29,  -20,  -30, -153 ,
     -31,   32,   29,    8,   22,   32,   45,  -33 ,
       5,   42,   49,   50,   48,   46,   49,    8 ,
       4,   35,   56,   68,   64,   57,   42,    9 ,
     -20,   15,   47,   69,   68,   48,   25,   -8 ,
     -16,    2,   28,   51,   48,   33,    7,  -12 ,
     -37,  -12,    6,   23,   20,   15,  -15,  -43 ,
    -103,  -63,  -36,  -41,  -57,  -26,  -64, -117 ,

};
const Score knightMobEg [9] = {-96, -31, 7, 27, 37, 49, 48, 44, 25, };
const Score bishopMobEg [14] = {-149, -84, -28, 0, 18, 36, 49, 54, 63, 64, 59, 50, 55, 26, };
const Score rookMobEg [15] = {-100, -53, -3, 9, 29, 47, 56, 58, 64, 69, 74, 77, 82, 69, 53, };
const Score queenMobEg [28] = {-219, -194, -128, -34, 72, 136, 177, 200, 222, 232, 238, 246, 250, 259, 265, 270, 274, 281, 282, 279, 277, 266, 256, 238, 212, 188, 130, 125, };

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