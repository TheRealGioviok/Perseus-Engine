#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {115, 406, 423, 585, 1249, 0};
const Score egValues [6] = {155, 409, 430, 764, 1257, 0};

const Score mgPawnTable [64] = {

       1,    4,    8,    7,   10,    2,    1,    9 ,
      26,   28,   16,   27,   20,   24,   15,   -4 ,
       2,    3,    6,    9,   19,   10,    3,   -5 ,
      -2,   -7,   -1,    5,    7,    4,   -6,   -1 ,
      -7,   -8,   -4,    4,    6,    6,   -1,   -1 ,
      -7,   -7,   -7,   -8,   -5,  -15,    2,    3 ,
     -30,  -29,  -22,  -22,  -20,  -17,   -3,  -17 ,
      -4,   -1,   -3,   -4,   -5,   -5,   -7,   -5 ,

};
const Score mgKnightTable [64] = {

    -148,  -94,  -55,  -14,   25,  -40,  -53,  -80 ,
       7,   40,   68,   94,   66,  153,   36,   55 ,
      12,   40,   60,   76,  144,  136,   89,   56 ,
      23,   30,   49,   87,   67,  109,   64,   91 ,
       2,   13,   20,   32,   55,   49,   79,   40 ,
     -34,  -16,    0,    5,   23,   14,   30,   12 ,
     -63,  -46,  -35,  -18,   -3,   -9,   -1,    6 ,
     -75,  -28,  -39,  -10,   -3,   -1,  -21,  -36 ,

};
const Score mgBishopTable [64] = {

     -11,  -45,  -43,  -86,  -61,  -60,  -23,  -37 ,
      14,   45,   24,   20,   54,   49,   31,   -7 ,
       3,   22,   28,   48,   50,  104,   64,   57 ,
       0,   23,   36,   62,   61,   59,   45,   19 ,
       1,    1,   11,   45,   51,   16,   20,   45 ,
       4,   40,   19,   17,   18,   26,   49,   36 ,
       3,   -7,   25,  -19,    1,   15,   41,   41 ,
       9,   46,    9,   -5,    4,    4,   22,   26 ,

};
const Score mgRookTable [64] = {

      15,   -2,   -5,   -5,   13,   42,   41,   65 ,
      37,   36,   48,   75,   58,  101,   82,  103 ,
       0,   34,   30,   36,   88,  103,  129,  103 ,
       1,   16,   21,   22,   28,   51,   60,   65 ,
     -20,  -17,  -12,   -6,   -5,   -4,   44,   29 ,
     -24,  -20,  -20,  -23,  -12,    5,   70,   36 ,
     -48,  -45,  -23,  -27,  -14,    4,   44,   -3 ,
      -7,   -6,   -8,   -1,    8,   17,   22,    1 ,

};
const Score mgQueenTable [64] = {

     -11,  -15,   17,   52,   56,   80,   88,   41 ,
      44,    9,   16,   25,   37,   81,   66,  122 ,
      26,   25,   16,   38,   52,  137,  119,  127 ,
      19,   14,   27,   21,   23,   58,   62,   76 ,
      12,   10,    7,   17,   24,   23,   47,   62 ,
      24,   23,   16,    4,    4,   27,   61,   61 ,
       3,   -2,   15,   15,   16,   28,   47,   77 ,
       7,   30,   30,   32,   30,    9,   31,   29 ,

};
const Score mgKingTable [64] = {

       8,   31,   41,    0,   17,   45,   73,   45 ,
      18,   74,   35,   90,   68,   88,  116,   73 ,
     -14,   84,   33,   22,   60,  124,  102,   61 ,
       6,   11,  -13,  -59,  -50,   -4,   13,  -29 ,
      -2,   -3,  -42,  -90,  -82,  -36,  -40,  -73 ,
      33,   55,  -20,  -45,  -44,  -37,   16,   13 ,
     117,   49,   49,    1,    4,   14,   96,  115 ,
     109,  136,  130,   19,  100,   42,  131,  137 ,

};

const Score egPawnTable [64] = {

       1,   -5,   -2,   -1,   -1,    0,   -5,   -6 ,
      34,   32,   31,   26,   28,   28,   27,   33 ,
      20,   21,   17,   12,   13,   15,   17,   14 ,
      14,   13,    3,   -5,   -7,   -1,    4,    3 ,
       8,    8,   -5,  -10,   -9,   -8,    0,   -3 ,
     -11,  -17,  -19,  -19,  -16,  -14,  -17,  -19 ,
     -25,  -28,  -26,  -28,  -28,  -22,  -31,  -35 ,
       0,   -2,   -1,   -3,    2,   -1,    5,    2 ,

};
const Score egKnightTable [64] = {

     -59,  -11,   14,    0,    2,  -29,  -21,  -86 ,
      42,   50,   42,   35,   20,    9,   37,    9 ,
      32,   37,   58,   51,   28,   17,   12,    8 ,
      27,   34,   51,   50,   53,   43,   36,   13 ,
      30,   25,   48,   45,   55,   36,   23,   23 ,
     -10,    0,    6,   35,   30,    4,   -3,    0 ,
     -28,  -20,  -15,   -9,  -17,  -16,  -26,  -18 ,
      -8,   -6,    5,   14,   16,    2,    2,   14 ,

};
const Score egBishopTable [64] = {

      24,   34,   29,   38,   27,   16,   18,    7 ,
      45,   51,   53,   46,   31,   39,   48,   41 ,
      55,   46,   44,   27,   26,   35,   35,   39 ,
      34,   33,   28,   41,   30,   30,   24,   31 ,
      18,   29,   37,   32,   31,   28,   25,    2 ,
       3,    9,   15,   23,   30,   16,    1,   -2 ,
      -9,  -23,  -28,    0,   -4,  -10,  -14,  -31 ,
       9,   24,   21,   19,   22,   34,   12,  -17 ,

};
const Score egRookTable [64] = {

      65,   68,   77,   71,   61,   59,   58,   48 ,
      98,  113,  111,   91,   91,   80,   84,   72 ,
      84,   84,   84,   73,   58,   51,   52,   40 ,
      72,   67,   76,   65,   51,   48,   48,   39 ,
      57,   58,   55,   48,   52,   49,   37,   31 ,
      31,   27,   22,   26,   17,    8,  -17,   -6 ,
       6,    8,    6,    7,   -7,  -14,  -28,  -19 ,
      37,   32,   37,   25,   18,   23,   18,   21 ,

};
const Score egQueenTable [64] = {

      96,  108,  130,  120,  127,  109,   69,   96 ,
     135,  171,  193,  200,  219,  177,  163,  161 ,
     127,  135,  181,  185,  196,  172,  136,  142 ,
     114,  134,  143,  172,  200,  181,  177,  159 ,
     100,  126,  129,  157,  157,  157,  141,  136 ,
      53,   86,   95,  109,  122,  113,   90,   78 ,
      28,   35,   44,   51,   56,   34,   11,  -17 ,
      49,   46,   51,   68,   54,   57,   41,   30 ,

};
const Score egKingTable [64] = {

     -59,   -7,    5,   36,   28,   40,   35,  -59 ,
      58,  105,  103,   89,  109,  123,  119,   79 ,
      58,  100,  115,  120,  128,  123,  119,   69 ,
      33,   79,  106,  118,  120,  115,   94,   55 ,
      12,   57,   89,  111,  110,   92,   74,   47 ,
     -10,   22,   51,   70,   71,   60,   35,   14 ,
     -45,   -8,    6,   24,   29,   21,   -7,  -43 ,
     -60,  -31,  -13,    8,  -12,   10,  -23,  -75 ,

};

extern Score* mgPestoTables[6];

extern Score* egPestoTables[6];

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };
extern Score mgTables[12][64];
extern Score egTables[12][64];

constexpr Score knightMobMg [9]  = {-45, -5, 20, 35, 49, 60, 73, 86, 93, };
constexpr Score bishopMobMg [14] = {-57, -5, 15, 28, 45, 58, 66, 71, 74, 80, 95, 103, 111, 97, };
constexpr Score rookMobMg   [15] = {-13, -8, 6, 6, 13, 10, 15, 22, 27, 41, 43, 47, 53, 62, 52, };
constexpr Score queenMobMg  [28] = {-23, -113, -29, 0, 15, 23, 33, 38, 46, 51, 59, 65, 69, 75, 78, 85, 86, 87, 93, 111, 117, 142, 161, 169, 190, 184, 123, 105, };

constexpr Score knightMobEg [9]  = {-51, 18, 50, 62, 71, 85, 82, 79, 64, };
constexpr Score bishopMobEg [14] = {-25, -16, 14, 37, 53, 67, 77, 81, 87, 88, 83, 82, 82, 73, };
constexpr Score rookMobEg   [15] = {-81, 31, 44, 65, 73, 86, 92, 94, 99, 104, 109, 113, 117, 112, 116, };
constexpr Score queenMobEg  [28] = {-10, -82, -52, -12, 46, 79, 119, 146, 169, 178, 189, 199, 210, 220, 228, 230, 247, 253, 256, 251, 255, 240, 223, 214, 205, 194, 158, 144, };

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