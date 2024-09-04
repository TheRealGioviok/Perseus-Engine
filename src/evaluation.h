#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {118, 408, 426, 591, 1288, 0};
const Score egValues [6] = {155, 417, 435, 768, 1296, 0};

const Score mgPawnTable [64] = {

       2,    4,    8,    7,   10,    3,    1,    8 ,
      30,   32,   18,   31,   23,   28,   15,   -4 ,
       3,    2,    7,   10,   22,   12,    3,   -5 ,
      -2,   -4,    0,    6,   10,    5,   -1,   -1 ,
      -9,  -10,   -5,    4,    8,    4,   -3,   -3 ,
      -8,   -7,   -8,   -9,   -7,  -16,    3,    1 ,
     -34,  -34,  -26,  -27,  -24,  -21,   -3,  -18 ,
      -4,   -1,   -3,   -4,   -5,   -5,   -5,   -5 ,

};
const Score mgKnightTable [64] = {

    -150,  -93,  -56,  -11,   25,  -41,  -54,  -80 ,
      10,   43,   69,   98,   68,  157,   38,   57 ,
       9,   44,   61,   80,  145,  139,   87,   53 ,
      20,   17,   41,   81,   57,   91,   45,   81 ,
       2,    5,   14,   25,   46,   39,   64,   36 ,
     -34,  -17,   -1,    5,   21,   12,   30,   12 ,
     -65,  -48,  -35,  -18,   -1,  -11,    0,    8 ,
     -75,  -23,  -39,   -7,    0,    2,  -16,  -37 ,

};
const Score mgBishopTable [64] = {

     -12,  -43,  -43,  -85,  -61,  -57,  -21,  -36 ,
      17,   47,   26,   24,   58,   53,   31,   -4 ,
       2,   25,   30,   56,   53,  102,   59,   51 ,
      -3,    8,   32,   62,   54,   45,   24,    9 ,
      -1,   -5,    9,   42,   44,   10,   11,   39 ,
       5,   39,   18,   16,   16,   24,   48,   36 ,
       2,   -9,   26,  -21,    0,   13,   44,   43 ,
       9,   48,   13,   -3,    5,    7,   24,   25 ,

};
const Score mgRookTable [64] = {

      16,    0,   -3,   -2,   16,   43,   43,   67 ,
      40,   39,   49,   79,   60,  102,   82,  105 ,
      -1,   33,   29,   37,   87,  102,  128,  103 ,
      -1,    9,   12,   17,   17,   37,   50,   57 ,
     -23,  -26,  -18,  -12,  -14,  -15,   30,   26 ,
     -27,  -23,  -23,  -27,  -17,    0,   64,   32 ,
     -54,  -49,  -26,  -32,  -18,   -2,   40,   -4 ,
      -7,   -7,   -8,   -1,    8,   16,   22,    2 ,

};
const Score mgQueenTable [64] = {

     -13,  -17,   16,   50,   54,   76,   83,   41 ,
      42,    6,   12,   25,   36,   80,   61,  118 ,
      22,   20,   12,   35,   53,  134,  115,  126 ,
      11,    4,   15,   13,   11,   43,   49,   60 ,
       7,    0,    2,    9,   14,   14,   35,   56 ,
      20,   21,   11,    1,   -1,   22,   56,   57 ,
      -3,   -8,   12,   10,   13,   22,   44,   72 ,
       6,   28,   28,   32,   28,    6,   28,   24 ,

};
const Score mgKingTable [64] = {

      12,   36,   43,    2,   18,   45,   76,   46 ,
      31,   85,   45,   94,   73,   94,  122,   82 ,
      -3,   92,   40,   28,   64,  127,  109,   70 ,
      13,   20,   -5,  -51,  -42,    4,   23,  -19 ,
       4,    4,  -34,  -81,  -74,  -30,  -33,  -66 ,
      39,   64,  -12,  -37,  -37,  -28,   28,   21 ,
     124,   58,   57,    7,   12,   23,  108,  127 ,
     120,  149,  141,   29,  113,   53,  144,  148 ,

};

const Score egPawnTable [64] = {

       0,   -5,   -2,   -1,   -1,    0,   -4,   -5 ,
      39,   38,   36,   29,   32,   32,   33,   40 ,
      23,   24,   20,   13,   14,   17,   19,   17 ,
      14,   13,    3,   -5,   -7,   -1,    4,    4 ,
       7,    7,   -5,  -11,  -10,   -8,   -1,   -3 ,
     -15,  -20,  -22,  -21,  -17,  -15,  -20,  -22 ,
     -30,  -34,  -31,  -31,  -31,  -26,  -36,  -42 ,
       0,   -2,   -1,   -3,    2,   -1,    4,    1 ,

};
const Score egKnightTable [64] = {

     -57,  -13,   13,   -1,    0,  -30,  -23,  -84 ,
      47,   55,   44,   37,   24,   10,   40,   13 ,
      34,   33,   59,   49,   26,   15,   10,    9 ,
      25,   29,   47,   47,   46,   39,   28,   10 ,
      27,   19,   44,   40,   51,   31,   15,   19 ,
     -14,   -6,    1,   31,   26,   -1,  -10,   -6 ,
     -34,  -26,  -21,  -14,  -22,  -22,  -33,  -23 ,
      -8,   -7,    5,   14,   16,    0,    2,   12 ,

};
const Score egBishopTable [64] = {

      24,   33,   27,   36,   26,   14,   17,    6 ,
      50,   56,   56,   49,   34,   41,   52,   46 ,
      57,   45,   42,   22,   22,   31,   35,   42 ,
      33,   32,   22,   34,   23,   24,   21,   30 ,
      16,   26,   31,   26,   25,   23,   21,   -1 ,
      -1,    6,   12,   20,   28,   14,   -2,   -6 ,
     -16,  -29,  -33,   -4,   -9,  -16,  -20,  -38 ,
       7,   20,   20,   19,   21,   33,   10,  -19 ,

};
const Score egRookTable [64] = {

      63,   67,   76,   69,   59,   58,   56,   47 ,
     102,  116,  114,   93,   93,   81,   87,   76 ,
      86,   84,   85,   74,   57,   50,   51,   41 ,
      71,   64,   74,   63,   47,   44,   44,   37 ,
      53,   54,   52,   45,   47,   44,   30,   26 ,
      26,   21,   17,   22,   14,    5,  -22,  -11 ,
      -1,    1,    1,    2,  -10,  -20,  -34,  -27 ,
      36,   30,   36,   24,   17,   22,   17,   19 ,

};
const Score egQueenTable [64] = {

      82,   93,  115,  106,  114,   97,   57,   85 ,
     125,  164,  184,  191,  208,  167,  154,  152 ,
     115,  122,  169,  171,  180,  157,  121,  128 ,
     100,  120,  128,  159,  181,  163,  160,  143 ,
      85,  109,  113,  142,  142,  141,  124,  119 ,
      36,   69,   80,   91,  105,   97,   75,   59 ,
       9,   16,   24,   34,   36,   15,   -9,  -33 ,
      36,   33,   39,   53,   41,   43,   28,   19 ,

};
const Score egKingTable [64] = {

     -53,    1,   14,   44,   35,   48,   43,  -52 ,
      71,  116,  115,  102,  120,  134,  130,   93 ,
      67,  109,  124,  128,  136,  131,  127,   79 ,
      40,   86,  114,  125,  127,  122,  102,   63 ,
      18,   63,   95,  118,  117,   99,   80,   54 ,
      -7,   26,   55,   75,   77,   66,   40,   17 ,
     -45,   -7,    9,   27,   31,   22,   -6,  -43 ,
     -55,  -26,   -8,   15,   -7,   16,  -18,  -70 ,

};

extern Score* mgPestoTables[6];

extern Score* egPestoTables[6];

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };
extern Score mgTables[12][64];
extern Score egTables[12][64];

constexpr Score knightMobMg [9] = {-47, -9, 15, 31, 45, 56, 70, 82, 92, };
constexpr Score bishopMobMg [14] = {-64, -10, 11, 24, 42, 55, 63, 69, 73, 78, 92, 100, 107, 94, };
constexpr Score rookMobMg [15] = {-15, -11, 3, 4, 10, 7, 12, 20, 25, 39, 41, 45, 51, 59, 50, };
constexpr Score queenMobMg [28] = {-22, -116, -37, -8, 8, 16, 25, 32, 39, 46, 53, 60, 64, 70, 73, 81, 82, 82, 88, 106, 113, 137, 155, 161, 178, 172, 112, 95, };

constexpr Score knightMobEg [9] = {-62, 11, 44, 57, 68, 81, 79, 76, 61, };
constexpr Score bishopMobEg [14] = {-29, -19, 9, 33, 49, 63, 72, 76, 83, 85, 79, 80, 78, 70, };
constexpr Score rookMobEg [15] = {-85, 30, 44, 63, 72, 85, 91, 93, 99, 104, 108, 113, 117, 111, 115, };
constexpr Score queenMobEg [28] = {-11, -83, -59, -28, 23, 54, 95, 124, 146, 156, 166, 176, 187, 195, 205, 206, 224, 230, 232, 227, 232, 216, 206, 196, 189, 179, 144, 132, };

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