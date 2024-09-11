#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {102, 368, 385, 537, 1069, 0};
const Score mgPawnTable [64] = {

       5,    8,    8,    6,    5,    3,   10,    8 ,
       3,    6,    5,    2,    2,    3,   12,    1 ,
      -4,    2,    2,    2,    4,    2,    4,   -3 ,
      -2,   -7,   -2,    3,    5,   -1,   -7,   -4 ,
      -1,   -1,    3,    9,    7,    8,    0,   -1 ,
       1,   -3,   -4,   -3,    2,   -4,   -1,    4 ,
      -4,   -6,   -8,   -5,   -7,   -3,   -7,  -10 ,
      -3,   -6,   -3,   -5,   -4,   -6,  -13,   -5 ,

};
const Score mgKnightTable [64] = {

    -135,  -94,  -70,   -8,   12,  -72,  -94,  -89 ,
     -14,   -6,   46,   99,   70,   97,    2,    5 ,
      10,   54,   61,   74,  109,   84,   78,   20 ,
      21,   36,   54,   66,   61,   74,   54,   36 ,
      12,   33,   30,   37,   49,   40,   57,   32 ,
     -30,   -8,    3,   10,   15,   13,    9,   -4 ,
     -29,  -45,  -22,   -3,    1,  -13,  -30,   -8 ,
     -50,  -33,  -18,  -21,  -12,  -16,  -30,  -47 ,

};
const Score mgBishopTable [64] = {

     -34,  -57,  -75,  -98,  -80, -102,  -31,  -37 ,
     -42,    2,    6,   -8,   20,   18,   11,  -27 ,
       8,   32,   31,   37,   43,   49,   48,   32 ,
       7,   24,   30,   51,   54,   37,   33,   13 ,
       6,   10,   20,   39,   45,   23,   19,   11 ,
      10,   25,   19,   27,   25,   25,   27,   23 ,
      21,   11,   12,    0,    4,   10,   21,   25 ,
      22,   13,    4,   -5,   -8,   -1,    1,   20 ,

};
const Score mgRookTable [64] = {

      37,   23,   11,   11,   20,   21,   48,   59 ,
      15,   17,   38,   52,   50,   61,   41,   37 ,
       6,   59,   39,   59,   79,   71,   93,   44 ,
       5,   27,   31,   45,   42,   43,   50,   36 ,
     -17,    0,   -4,   -5,   -9,   -2,   16,    2 ,
     -34,    0,  -15,  -17,  -11,   -9,   27,   -4 ,
     -67,  -30,  -18,  -12,  -11,    0,   -1,  -49 ,
     -24,  -17,  -11,   -6,   -5,    0,   -7,   -9 ,

};
const Score mgQueenTable [64] = {

      52,   38,   49,   67,   60,   67,   54,   50 ,
       9,  -30,    6,   -8,    6,   40,  -14,   11 ,
      32,   35,   23,   28,   35,   65,   81,   83 ,
      27,   34,   29,   19,   20,   37,   53,   60 ,
      21,   32,   19,   21,   28,   25,   43,   44 ,
      24,   29,   26,   21,   25,   30,   44,   41 ,
      14,   23,   31,   33,   30,   39,   23,   14 ,
      17,   30,   33,   36,   34,   34,   33,   21 ,

};
const Score mgKingTable [64] = {

     -18,   20,   15,   -3,   18,   29,   65,    4 ,
     -44,   29,   14,   62,   36,   67,   48,   31 ,
     -32,   72,   57,   31,   41,   99,   73,   10 ,
     -37,   17,   12,  -36,  -22,   17,   19,  -54 ,
     -48,   14,  -10,  -62,  -63,  -19,  -12,  -75 ,
     -34,  -16,  -35,  -74,  -70,  -58,  -38,  -47 ,
      44,   -4,  -29,  -82,  -73,  -56,   -4,   36 ,
      28,   38,   10,  -52,   -3,  -46,   42,   44 ,

};
constexpr Score knightMobMg [9] = {-29, -12, 6, 15, 23, 31, 40, 51, 60, };
constexpr Score bishopMobMg [14] = {-66, -24, 2, 11, 23, 29, 35, 38, 42, 48, 60, 84, 98, 113, };
constexpr Score rookMobMg [15] = {-16, -20, -8, -6, -2, -2, 3, 9, 14, 26, 30, 39, 42, 59, 91, };
constexpr Score queenMobMg [28] = {-107, -69, -27, -5, 1, 9, 17, 23, 29, 37, 44, 50, 56, 59, 61, 63, 65, 62, 65, 73, 79, 90, 103, 123, 145, 159, 135, 136, };

const Score egValues [6] = {156, 424, 452, 799, 1256, 0};
const Score egPawnTable [64] = {

      -4,   -5,   -3,   -1,   -2,   -1,   -7,   -7 ,
      -9,   -7,    0,    4,    3,    1,   -8,  -12 ,
      -4,    0,    1,    2,    3,    4,    0,   -6 ,
       4,    4,    0,   -6,   -7,   -2,    2,    1 ,
       8,    8,   -2,   -9,  -10,   -5,    5,    5 ,
       6,    1,   -2,   -7,   -7,   -3,    0,    2 ,
      11,    7,    7,    1,    1,    6,    5,    9 ,
       1,    3,    1,    1,    3,    1,    7,    5 ,

};
const Score egKnightTable [64] = {

     -52,    0,   32,   14,   11,   24,    6,  -79 ,
     -19,    8,   10,   17,   23,    0,    0,  -29 ,
      -1,   10,   50,   49,   39,   44,    1,   -3 ,
       7,   27,   53,   61,   63,   46,   31,    8 ,
      25,   26,   47,   53,   54,   45,   24,   18 ,
      -3,    6,   27,   40,   39,   24,    7,   -8 ,
       8,   16,    2,   13,   12,    0,    9,   13 ,
     -28,   -6,  -11,    1,    7,   -5,    1,  -31 ,

};
const Score egBishopTable [64] = {

      29,   31,   30,   37,   37,   28,   19,   19 ,
       9,   21,   24,   23,   13,   20,   19,    2 ,
      21,   26,   28,   21,   22,   27,   21,   15 ,
      17,   21,   23,   33,   32,   25,   18,   17 ,
      10,   17,   32,   29,   29,   27,   16,    8 ,
      10,   16,   23,   20,   21,   17,   10,   12 ,
       6,    0,    4,   17,   18,    8,    4,    1 ,
      -3,    7,   18,    4,   10,   20,   12,   -6 ,

};
const Score egRookTable [64] = {

      52,   55,   64,   61,   58,   62,   51,   44 ,
      41,   53,   55,   54,   54,   45,   47,   35 ,
      41,   30,   45,   38,   32,   35,   18,   28 ,
      36,   32,   41,   32,   33,   32,   27,   25 ,
      25,   35,   32,   26,   27,   31,   29,   20 ,
       9,   11,    7,    3,   -1,    2,   -1,   -7 ,
       9,    6,    3,   -5,   -8,  -10,  -13,   -3 ,
       4,    5,    5,   -4,   -5,    3,    3,   -5 ,

};
const Score egQueenTable [64] = {

      94,  123,  140,  152,  162,  144,  125,  113 ,
     108,  154,  169,  204,  217,  173,  166,  131 ,
     112,  116,  167,  187,  183,  185,  121,  123 ,
     114,  138,  148,  180,  192,  169,  156,  129 ,
      97,  119,  128,  154,  145,  143,  119,  112 ,
      67,   73,   91,   85,   82,   96,   76,   65 ,
      49,   31,   24,   41,   46,   17,   27,   38 ,
      23,   13,   11,   31,   17,   -7,    3,    5 ,

};
const Score egKingTable [64] = {

    -127,  -60,  -43,  -17,  -35,  -22,  -27, -131 ,
     -29,   33,   24,    4,   13,   27,   43,  -33 ,
       8,   49,   49,   45,   45,   47,   53,    7 ,
       1,   36,   53,   61,   59,   54,   41,    4 ,
     -17,   18,   41,   57,   57,   41,   26,   -7 ,
     -13,    3,   21,   38,   35,   24,    8,   -8 ,
     -29,   -4,    6,   16,   15,   12,   -7,  -33 ,
    -101,  -58,  -33,  -37,  -51,  -26,  -56, -110 ,

};
constexpr Score knightMobEg [9] = {-74, -15, 18, 38, 47, 60, 59, 54, 37, };
constexpr Score bishopMobEg [14] = {-130, -64, -10, 17, 34, 49, 60, 65, 73, 74, 69, 61, 60, 38, };
constexpr Score rookMobEg [15] = {-98, -49, 0, 14, 35, 51, 59, 59, 65, 70, 74, 76, 80, 69, 55, };
constexpr Score queenMobEg [28] = {-204, -202, -145, -60, 38, 102, 141, 165, 187, 197, 204, 213, 218, 226, 232, 237, 241, 248, 248, 247, 243, 233, 218, 203, 177, 160, 113, 112, };

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