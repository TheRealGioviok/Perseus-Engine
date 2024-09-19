#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {133, 477, 511, 685, 1110, 0};
const Score mgPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      66,   23,   70,   96,   91,   63,  -63,  -55 ,
     -11,   34,   59,   53,   63,   91,   57,   -9 ,
     -18,  -22,   -5,    6,   16,    7,  -23,  -22 ,
     -29,  -34,  -18,   -1,   -3,   -6,  -30,  -28 ,
     -27,  -22,  -19,  -12,   -2,  -18,  -11,  -23 ,
     -30,  -25,  -26,  -28,  -26,  -13,   -9,  -33 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score mgKnightTable [64] = {

    -200, -156, -126,  -50,  -11, -148, -172, -162 ,
     -17,  -11,   39,  101,   65,   91,  -25,   -1 ,
      -4,   48,   61,   60,   95,   71,   49,   -6 ,
      22,   35,   50,   63,   59,   69,   30,   29 ,
      20,   37,   37,   45,   58,   43,   55,   35 ,
     -24,    4,   16,   25,   30,   22,   17,    1 ,
     -21,  -31,   -4,   11,   14,    4,  -10,    3 ,
     -52,  -25,   -8,  -10,   -3,   -4,  -19,  -44 ,

};
const Score mgBishopTable [64] = {

     -46,  -88, -117, -151, -135, -169,  -61,  -69 ,
     -55,   -2,    5,  -23,    2,   -2,  -21,  -47 ,
       5,   28,   34,   25,   31,   37,   28,    4 ,
       2,   32,   30,   48,   54,   36,   36,   -4 ,
       7,    8,   15,   40,   47,   23,   18,   11 ,
       7,   27,   25,   30,   29,   31,   32,   24 ,
      22,   23,   23,   10,   15,   21,   41,   29 ,
      27,   16,    9,    2,   -2,    8,    9,   33 ,

};
const Score mgRookTable [64] = {

      17,    1,  -18,  -27,  -20,   -4,   27,   38 ,
       2,   -3,   19,   31,   13,   37,   10,   20 ,
       3,   61,   37,   63,   76,   61,   90,   31 ,
       2,   33,   39,   56,   48,   46,   53,   27 ,
     -32,  -13,  -15,   -9,  -14,  -18,    0,  -20 ,
     -43,   -8,  -21,  -17,  -15,  -18,   15,  -21 ,
     -69,  -24,  -14,   -8,   -8,    1,    5,  -59 ,
     -22,  -11,   -5,    3,    1,    3,    3,   -8 ,

};
const Score mgQueenTable [64] = {

      13,   12,   16,   30,   19,   22,   17,   14 ,
     -20,  -75,  -29,  -75,  -79,   -8,  -78,  -17 ,
      -5,   -8,  -16,  -17,  -14,   10,   18,   20 ,
      -3,    7,   -4,  -17,  -15,    5,   20,   14 ,
      -1,    1,  -15,   -9,   -3,   -7,   11,    9 ,
      -2,   11,    4,   -1,    3,    8,   20,   12 ,
      -5,   14,   21,   20,   17,   27,   19,   -6 ,
       3,   15,   19,   24,   22,   19,   23,    7 ,

};
const Score mgKingTable [64] = {

     -12,   29,   25,    1,   25,   33,   74,    7 ,
     -53,   35,   27,   82,   51,   82,   49,   28 ,
     -40,   87,   77,   46,   60,  122,   85,    7 ,
     -52,   23,   32,  -19,    0,   38,   23,  -74 ,
     -63,   23,   18,  -33,  -30,   10,    0,  -96 ,
     -49,   11,   11,  -23,  -15,   -7,    4,  -57 ,
      28,   11,    5,  -52,  -40,  -23,   24,   24 ,
       5,   21,   -8,  -69,  -29,  -57,   24,   23 ,

};
constexpr Score knightMobMg [9] = {-34, -15, -2, 5, 11, 17, 25, 35, 46, };
constexpr Score bishopMobMg [14] = {-87, -42, -18, -10, -1, 4, 7, 8, 8, 13, 23, 49, 59, 86, };
constexpr Score rookMobMg [15] = {-18, -22, -12, -10, -8, -9, -5, 1, 4, 14, 15, 22, 24, 43, 95, };
constexpr Score queenMobMg [28] = {-158, -116, -73, -48, -42, -35, -26, -20, -15, -9, -4, 2, 7, 9, 10, 11, 12, 7, 11, 21, 27, 46, 51, 73, 79, 112, 92, 117, };

const Score egValues [6] = {203, 570, 609, 1050, 1732, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      94,  106,   65,    8,    7,   41,  110,  116 ,
      60,   54,    9,  -32,  -35,   -6,   40,   53 ,
      15,    9,  -13,  -34,  -36,  -21,    6,    5 ,
     -12,  -12,  -26,  -37,  -39,  -28,  -18,  -19 ,
     -19,  -21,  -19,  -24,  -23,  -18,  -26,  -28 ,
     -11,  -10,    0,   -2,   -3,   -1,  -16,  -22 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -71,   -6,   30,    9,    0,   27,   10, -107 ,
     -34,   -2,   -3,    2,   10,  -13,  -10,  -42 ,
     -13,   -5,   40,   41,   30,   35,   -8,  -12 ,
      -1,   24,   50,   59,   61,   44,   35,    2 ,
      11,   12,   42,   48,   50,   41,   15,    5 ,
     -21,   -6,   23,   39,   39,   21,   -4,  -28 ,
      -8,    3,   -6,    7,    7,  -10,   -7,   -6 ,
     -39,  -21,  -22,   -9,   -3,  -14,  -16,  -52 ,

};
const Score egBishopTable [64] = {

      19,   17,   16,   22,   26,   17,    7,    9 ,
       2,   11,    8,    3,   -5,    4,   11,   -1 ,
      13,   11,   12,    2,    2,   13,    9,   10 ,
       2,    8,    9,   24,   21,   12,    6,    6 ,
     -13,   -1,   23,   22,   23,   18,   -2,  -15 ,
     -16,    0,   13,   14,   17,   11,   -6,  -15 ,
     -20,  -26,   -9,    4,    6,   -6,  -20,  -29 ,
     -22,   -6,    3,  -10,   -5,    6,   -5,  -30 ,

};
const Score egRookTable [64] = {

      46,   52,   60,   59,   56,   56,   48,   41 ,
      37,   46,   44,   38,   43,   33,   40,   32 ,
      29,    8,   20,    7,    1,   10,   -3,   19 ,
      23,   15,   19,    7,    7,    8,   10,   13 ,
       8,   17,   17,   12,   11,   19,   15,    4 ,
     -12,   -4,   -3,   -5,   -8,   -5,  -15,  -29 ,
     -14,  -16,  -12,  -17,  -20,  -27,  -37,  -31 ,
     -13,  -13,  -10,  -21,  -23,  -12,  -19,  -27 ,

};
const Score egQueenTable [64] = {

     -58,  -29,  -10,  -20,  -12,  -14,  -32,  -56 ,
       0,   65,   53,   76,   83,   30,   62,    8 ,
      21,   25,   64,   64,   52,   36,  -13,  -14 ,
      25,   57,   58,   91,   84,   55,   48,   10 ,
       6,   43,   61,   95,   83,   64,   25,   -3 ,
     -22,    4,   38,   40,   34,   40,    3,  -38 ,
     -35,  -40,  -34,   -8,   -3,  -47,  -47,  -51 ,
     -52,  -52,  -45,  -14,  -40,  -73,  -70,  -77 ,

};
const Score egKingTable [64] = {

    -178,  -78,  -49,  -11,  -32,  -25,  -37, -193 ,
     -26,   46,   40,   13,   28,   42,   58,  -33 ,
      13,   59,   58,   55,   56,   55,   65,   13 ,
       4,   42,   59,   70,   65,   58,   46,    6 ,
     -25,   14,   42,   64,   62,   40,   19,  -15 ,
     -22,   -3,   21,   44,   41,   23,   -2,  -20 ,
     -40,  -13,    5,   22,   18,   13,  -17,  -46 ,
    -118,  -57,  -23,  -32,  -44,  -16,  -55, -127 ,

};
constexpr Score knightMobEg [9] = {-114, -38, 5, 28, 39, 52, 51, 45, 23, };
constexpr Score bishopMobEg [14] = {-168, -94, -30, 1, 20, 38, 49, 52, 60, 58, 50, 36, 40, 0, };
constexpr Score rookMobEg [15] = {-135, -81, -21, -7, 15, 35, 44, 43, 49, 51, 55, 57, 60, 42, 16, };
constexpr Score queenMobEg [28] = {-353, -335, -254, -161, -52, 9, 46, 65, 83, 91, 95, 99, 99, 103, 104, 104, 102, 106, 99, 87, 78, 52, 39, 10, -2, -45, -13, -6, };

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