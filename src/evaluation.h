#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {115, 419, 445, 604, 1185, 0};
const Score mgPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      42,    1,   35,   63,   55,   29,  -66,  -64 ,
      -5,   30,   51,   48,   59,   85,   52,   -4 ,
      -9,  -16,    0,   12,   22,   16,  -14,  -11 ,
     -23,  -30,  -14,    3,    2,   -1,  -24,  -20 ,
     -21,  -16,  -14,   -6,    4,  -11,   -4,  -15 ,
     -22,  -18,  -18,  -18,  -15,   -5,    0,  -25 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score mgKnightTable [64] = {

    -171, -116,  -89,  -16,    4,  -86, -124, -127 ,
     -13,  -17,   37,  103,   66,   98,  -21,    0 ,
      11,   49,   58,   70,  106,   82,   73,   21 ,
      17,   37,   54,   66,   61,   77,   55,   32 ,
       5,   28,   22,   31,   43,   31,   51,   23 ,
     -40,  -15,   -4,    5,   10,    6,    2,  -14 ,
     -37,  -49,  -22,   -8,   -4,  -15,  -26,  -15 ,
     -63,  -38,  -24,  -26,  -20,  -22,  -30,  -54 ,

};
const Score mgBishopTable [64] = {

     -47,  -76,  -94, -112,  -94, -113,  -50,  -52 ,
     -43,   -9,   -4,  -19,   12,    8,   -8,  -32 ,
      10,   29,   26,   30,   36,   41,   45,   31 ,
       3,   25,   31,   50,   52,   36,   32,    7 ,
       1,    1,   12,   32,   38,   13,   10,    5 ,
       3,   21,   14,   20,   20,   19,   23,   18 ,
      18,   11,   12,   -3,    1,    8,   28,   20 ,
      19,   10,    0,   -9,  -13,   -4,    2,   21 ,

};
const Score mgRookTable [64] = {

      37,   15,    2,    9,   18,   20,   40,   59 ,
       7,    3,   24,   42,   39,   54,   26,   28 ,
      -2,   52,   29,   51,   72,   66,   92,   37 ,
      -7,   23,   25,   39,   37,   39,   49,   28 ,
     -39,  -17,  -21,  -20,  -24,  -19,    2,  -17 ,
     -53,  -13,  -28,  -30,  -25,  -22,   18,  -22 ,
     -81,  -39,  -25,  -24,  -20,  -10,   -1,  -62 ,
     -35,  -24,  -20,  -13,  -13,   -9,   -7,  -17 ,

};
const Score mgQueenTable [64] = {

      52,   28,   35,   57,   56,   60,   40,   40 ,
       5,  -51,  -13,  -30,  -15,   25,  -49,    1 ,
      30,   31,    9,   15,   25,   52,   79,   80 ,
      18,   30,   25,   10,   10,   32,   48,   53 ,
      12,   20,    6,    7,   15,   11,   30,   34 ,
      12,   22,   17,   11,   17,   20,   37,   28 ,
       6,   21,   28,   26,   24,   32,   26,    7 ,
       9,   24,   28,   30,   28,   28,   33,   16 ,

};
const Score mgKingTable [64] = {

      -4,   39,   35,    3,   29,   37,   76,    8 ,
     -41,   37,   17,   79,   44,   82,   53,   46 ,
     -37,   98,   69,   31,   43,  119,   93,   15 ,
     -53,   23,   15,  -39,  -20,   17,   28,  -68 ,
     -60,    3,  -21,  -78,  -77,  -33,  -27,  -92 ,
     -44,  -25,  -44,  -88,  -82,  -70,  -43,  -61 ,
      47,   -1,  -29,  -94,  -84,  -64,    4,   37 ,
      35,   44,   12,  -52,   -5,  -51,   47,   52 ,

};
const Score knightMobMg [9] = {-31, -16, -2, 7, 15, 22, 31, 41, 51, };
const Score bishopMobMg [14] = {-83, -39, -14, -4, 7, 13, 19, 22, 27, 33, 48, 74, 87, 107, };
const Score rookMobMg [15] = {-24, -29, -16, -13, -11, -12, -8, -1, 3, 14, 18, 28, 32, 50, 88, };
const Score queenMobMg [28] = {-132, -89, -44, -23, -17, -10, -1, 4, 9, 16, 22, 28, 33, 37, 38, 41, 43, 40, 42, 51, 58, 70, 79, 104, 128, 147, 139, 130, };

const Score egValues [6] = {177, 495, 528, 914, 1587, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      65,   72,   37,  -18,  -19,   11,   72,   81 ,
      57,   50,    7,  -32,  -38,  -12,   33,   49 ,
      20,   13,   -9,  -31,  -34,  -20,    7,   10 ,
      -2,   -5,  -19,  -30,  -32,  -22,  -13,  -11 ,
      -9,  -14,  -11,  -16,  -15,  -11,  -18,  -18 ,
      -1,   -1,    7,    6,    5,    7,   -8,  -11 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -68,   -9,   21,    3,    5,   17,    5,  -93 ,
     -26,    3,    1,    5,   16,  -10,   -2,  -29 ,
     -10,   -1,   34,   36,   25,   28,   -8,   -9 ,
       1,   19,   43,   53,   55,   35,   24,    4 ,
      14,   13,   39,   46,   49,   38,   13,    8 ,
     -16,   -6,   20,   37,   38,   21,   -3,  -22 ,
      -4,    3,   -9,    8,    8,   -9,   -8,   -3 ,
     -29,  -18,  -19,   -6,   -1,  -12,  -14,  -40 ,

};
const Score egBishopTable [64] = {

      22,   17,   16,   23,   25,   14,    8,   15 ,
       7,   12,   11,    7,   -3,    8,   12,    3 ,
      14,   10,   11,    2,    5,   12,    7,   12 ,
       8,   10,    6,   19,   19,   10,    9,   11 ,
      -4,    3,   19,   16,   17,   16,    3,   -5 ,
      -6,    1,    9,   11,   12,    7,   -5,   -6 ,
     -11,  -21,  -11,    4,    5,   -7,  -18,  -17 ,
     -14,   -4,    6,  -10,   -4,    8,   -2,  -21 ,

};
const Score egRookTable [64] = {

      36,   46,   55,   51,   50,   50,   44,   33 ,
      33,   41,   39,   32,   34,   28,   34,   28 ,
      29,   11,   23,   14,    6,   11,   -1,   17 ,
      23,   16,   21,   13,   12,   11,   11,   12 ,
       6,   16,   17,   15,   15,   18,   13,    1 ,
     -14,   -4,   -2,   -3,   -5,   -4,  -15,  -29 ,
     -13,  -14,  -11,  -13,  -17,  -22,  -33,  -27 ,
     -12,  -11,   -8,  -16,  -19,   -8,  -17,  -25 ,

};
const Score egQueenTable [64] = {

      37,   71,   96,  109,  115,   99,   80,   68 ,
      53,  101,  116,  152,  168,  119,  123,   84 ,
      55,   49,  110,  128,  124,  129,   58,   75 ,
      65,   81,   84,  124,  141,  112,  106,   81 ,
      33,   61,   73,  102,   94,   92,   61,   54 ,
      -1,   12,   38,   35,   30,   46,   17,    1 ,
     -19,  -37,  -39,  -13,   -9,  -44,  -42,  -30 ,
     -35,  -46,  -48,  -24,  -40,  -62,  -59,  -59 ,

};
const Score egKingTable [64] = {

    -154,  -72,  -49,  -12,  -31,  -21,  -28, -155 ,
     -32,   30,   29,    7,   21,   33,   45,  -33 ,
       6,   41,   48,   49,   48,   46,   49,    7 ,
       4,   34,   56,   67,   64,   57,   41,    9 ,
     -21,   16,   47,   69,   68,   49,   26,   -8 ,
     -17,    3,   29,   52,   49,   33,    7,  -12 ,
     -38,  -12,    6,   23,   20,   15,  -15,  -43 ,
    -106,  -63,  -36,  -41,  -57,  -26,  -65, -119 ,

};
const Score knightMobEg [9] = {-101, -34, 3, 25, 35, 47, 46, 41, 22, };
const Score bishopMobEg [14] = {-152, -87, -30, -2, 17, 34, 46, 52, 61, 63, 58, 48, 54, 24, };
const Score rookMobEg [15] = {-121, -70, -21, -10, 10, 29, 38, 40, 46, 50, 56, 59, 64, 51, 35, };
const Score queenMobEg [28] = {-331, -309, -238, -149, -36, 29, 70, 95, 117, 126, 133, 142, 146, 155, 161, 166, 169, 177, 177, 174, 170, 159, 149, 130, 98, 73, 13, 8, };


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