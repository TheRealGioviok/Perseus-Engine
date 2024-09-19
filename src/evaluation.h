#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {120, 424, 454, 618, 1072, 0};
const Score mgPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      61,   20,   64,   85,   79,   55,  -59,  -49 ,
      -8,   30,   50,   44,   52,   77,   48,   -6 ,
     -15,  -19,   -3,    5,   14,    6,  -21,  -19 ,
     -24,  -29,  -15,    1,   -2,   -4,  -27,  -23 ,
     -22,  -18,  -17,  -10,   -1,  -16,   -8,  -19 ,
     -24,  -21,  -22,  -24,  -23,  -10,   -7,  -27 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score mgKnightTable [64] = {

    -180, -137, -113,  -48,  -14, -133, -152, -147 ,
     -14,   -9,   31,   87,   55,   74,  -24,    0 ,
      -6,   40,   51,   47,   75,   56,   35,  -10 ,
      21,   31,   45,   58,   54,   62,   25,   26 ,
      19,   34,   35,   42,   53,   40,   49,   33 ,
     -21,    4,   15,   24,   28,   22,   17,    2 ,
     -16,  -25,   -3,   11,   14,    4,   -8,    3 ,
     -45,  -21,   -7,   -8,   -2,   -2,  -14,  -37 ,

};
const Score mgBishopTable [64] = {

     -45,  -78, -105, -137, -121, -153,  -54,  -66 ,
     -49,   -2,    5,  -22,    0,   -7,  -21,  -43 ,
       4,   23,   29,   20,   23,   28,   22,    2 ,
       2,   30,   28,   43,   49,   31,   33,   -3 ,
       7,    8,   14,   37,   44,   22,   17,    9 ,
       7,   25,   24,   29,   28,   30,   30,   22 ,
      20,   21,   21,   10,   14,   20,   39,   27 ,
      25,   15,   10,    2,   -1,    8,    8,   29 ,

};
const Score mgRookTable [64] = {

      17,    6,   -9,  -19,  -14,    0,   29,   36 ,
       2,   -3,   17,   26,    9,   30,    7,   18 ,
       5,   57,   35,   58,   68,   54,   80,   28 ,
       4,   32,   37,   52,   46,   44,   49,   26 ,
     -28,  -10,  -12,   -6,  -11,  -15,    1,  -17 ,
     -38,   -6,  -17,  -14,  -12,  -15,   15,  -17 ,
     -61,  -20,  -11,   -6,   -6,    2,    6,  -50 ,
     -19,   -9,   -3,    4,    2,    5,    4,   -7 ,

};
const Score mgQueenTable [64] = {

      -3,    4,    4,   10,   -1,    9,    5,   -2 ,
     -16,  -66,  -30,  -80,  -90,  -21,  -74,  -14 ,
      -3,   -8,  -17,  -22,  -21,   -3,    3,    7 ,
       1,   10,   -3,  -15,  -16,    2,   18,   10 ,
       3,    5,  -10,   -3,    3,   -3,   12,   11 ,
       3,   15,    9,    5,    8,   13,   22,   15 ,
       0,   17,   25,   24,   21,   30,   22,   -1 ,
       6,   19,   23,   29,   25,   23,   26,    9 ,

};
const Score mgKingTable [64] = {

      -8,   31,   27,    3,   25,   31,   69,    9 ,
     -53,   33,   31,   87,   53,   82,   43,   19 ,
     -41,   85,   85,   54,   68,  128,   83,    4 ,
     -57,   31,   50,    6,   25,   56,   31,  -78 ,
     -61,   35,   50,   10,   14,   45,   20,  -92 ,
     -49,   25,   40,   18,   26,   28,   20,  -53 ,
      17,    6,    6,  -39,  -30,  -17,   18,   14 ,
      -9,   11,  -15,  -68,  -33,  -57,   14,    9 ,

};
constexpr Score knightMobMg [9] = {-27, -13, 0, 7, 13, 19, 27, 36, 46, };
constexpr Score bishopMobMg [14] = {-76, -36, -12, -4, 5, 10, 13, 13, 14, 17, 26, 48, 56, 80, };
constexpr Score rookMobMg [15] = {-15, -20, -10, -7, -6, -6, -2, 4, 7, 16, 16, 24, 24, 40, 86, };
constexpr Score queenMobMg [28] = {-133, -99, -60, -38, -34, -28, -20, -15, -10, -3, 2, 8, 12, 15, 15, 16, 16, 11, 13, 20, 24, 41, 43, 62, 67, 96, 78, 99, };

const Score egValues [6] = {189, 513, 547, 956, 1538, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      84,   95,   60,   10,   11,   39,  100,  104 ,
      60,   55,   16,  -19,  -22,    5,   43,   53 ,
      16,   12,   -7,  -26,  -28,  -13,   10,    7 ,
     -11,  -10,  -22,  -33,  -34,  -24,  -16,  -18 ,
     -18,  -19,  -16,  -21,  -20,  -15,  -23,  -26 ,
     -10,   -8,    0,   -1,   -2,    0,  -14,  -20 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -69,   -8,   25,    6,   -1,   23,    7,  -97 ,
     -28,   -1,   -1,    4,   11,   -9,   -7,  -34 ,
      -8,   -1,   39,   42,   33,   36,   -2,   -5 ,
       2,   25,   49,   57,   59,   42,   37,    5 ,
      12,   14,   40,   47,   49,   39,   17,    7 ,
     -19,   -5,   22,   36,   37,   21,   -1,  -23 ,
      -6,    2,   -7,    8,    7,   -9,   -6,   -3 ,
     -32,  -18,  -18,   -7,   -2,  -12,  -15,  -44 ,

};
const Score egBishopTable [64] = {

      16,   15,   13,   20,   22,   15,    6,    7 ,
       6,   13,    9,    4,   -3,    7,   14,    4 ,
      16,   14,   14,    5,    5,   16,   13,   14 ,
       5,   10,   11,   26,   24,   14,    8,    8 ,
     -11,    1,   24,   24,   25,   19,    0,  -12 ,
     -13,    3,   14,   16,   18,   13,   -2,  -12 ,
     -17,  -21,   -7,    6,    8,   -3,  -15,  -25 ,
     -19,   -4,    4,   -8,   -2,    7,   -2,  -26 ,

};
const Score egRookTable [64] = {

      44,   48,   55,   54,   53,   52,   45,   40 ,
      38,   47,   45,   41,   47,   37,   42,   34 ,
      31,   12,   23,   12,    7,   16,    3,   22 ,
      25,   18,   22,   12,   11,   12,   14,   16 ,
       9,   19,   19,   15,   14,   21,   18,    6 ,
     -10,    0,    0,   -3,   -4,   -2,   -9,  -24 ,
     -12,  -13,   -9,  -13,  -16,  -21,  -30,  -25 ,
     -11,  -11,   -8,  -16,  -19,   -9,  -16,  -24 ,

};
const Score egQueenTable [64] = {

     -45,  -25,   -5,   -9,   -1,   -6,  -24,  -42 ,
       1,   60,   52,   83,  101,   43,   65,   11 ,
      23,   28,   62,   66,   58,   50,   11,   15 ,
      23,   51,   55,   87,   83,   56,   49,   20 ,
       0,   38,   55,   85,   73,   59,   25,   -1 ,
     -23,    0,   32,   32,   26,   33,    4,  -36 ,
     -33,  -40,  -38,  -12,   -9,  -48,  -46,  -47 ,
     -51,  -52,  -46,  -20,  -41,  -70,  -65,  -71 ,

};
const Score egKingTable [64] = {

    -154,  -66,  -39,   -6,  -24,  -19,  -30, -167 ,
     -20,   43,   38,   14,   27,   39,   54,  -27 ,
      16,   55,   52,   49,   49,   49,   60,   13 ,
       6,   38,   51,   60,   55,   49,   41,    8 ,
     -22,   11,   33,   51,   49,   30,   13,  -15 ,
     -19,   -7,   12,   32,   28,   13,   -7,  -19 ,
     -31,   -9,    5,   18,   15,   11,  -13,  -38 ,
     -95,  -46,  -15,  -25,  -35,  -12,  -44, -108 ,

};
constexpr Score knightMobEg [9] = {-99, -31, 8, 30, 40, 53, 51, 46, 25, };
constexpr Score bishopMobEg [14] = {-146, -79, -23, 5, 23, 40, 50, 54, 61, 58, 51, 38, 40, 5, };
constexpr Score rookMobEg [15] = {-118, -72, -17, -5, 16, 34, 43, 43, 48, 52, 56, 58, 61, 45, 24, };
constexpr Score queenMobEg [28] = {-305, -293, -220, -140, -43, 9, 42, 58, 75, 80, 83, 87, 87, 90, 92, 92, 90, 93, 88, 78, 69, 46, 35, 9, 0, -38, -12, -8, };

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