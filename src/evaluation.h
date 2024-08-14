#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {120, 421, 437, 605, 1317, 0};
const Score egValues [6] = {156, 427, 443, 774, 1326, 0};

const Score mgPawnTable [64] = {

       2,    3,    7,    6,    9,    2,   -1,    7 ,
      32,   34,   19,   33,   23,   30,   14,   -5 ,
       3,    2,    7,   10,   23,   12,    3,   -5 ,
      -1,   -4,    0,    5,    9,    6,   -1,    0 ,
      -9,  -10,   -5,    3,    7,    4,   -3,   -2 ,
      -8,   -6,   -8,   -9,   -7,  -15,    4,    1 ,
     -37,  -37,  -27,  -29,  -25,  -22,   -2,  -18 ,
      -3,    0,   -2,   -3,   -4,   -4,   -4,   -4 ,

};
const Score mgKnightTable [64] = {

    -155,  -94,  -58,  -12,   24,  -44,  -55,  -84 ,
      11,   44,   70,  100,   67,  160,   37,   57 ,
       7,   51,   65,   92,  154,  148,   90,   52 ,
      18,   20,   50,   90,   60,   94,   45,   80 ,
      -1,    6,   19,   31,   48,   41,   63,   34 ,
     -38,  -20,   -4,    1,   17,    9,   28,    9 ,
     -70,  -54,  -38,  -22,   -6,  -16,   -2,    5 ,
     -77,  -25,  -40,   -8,   -2,    0,  -18,  -39 ,

};
const Score mgBishopTable [64] = {

     -15,  -44,  -46,  -87,  -63,  -60,  -20,  -37 ,
      18,   50,   27,   25,   58,   54,   31,   -6 ,
       0,   29,   35,   65,   60,  108,   60,   50 ,
      -5,   11,   38,   72,   60,   48,   24,    7 ,
      -2,   -3,   11,   48,   47,   10,   10,   38 ,
       3,   38,   18,   15,   13,   23,   47,   33 ,
      -2,  -13,   23,  -24,   -3,    9,   43,   43 ,
       9,   48,   12,   -4,    2,    7,   24,   26 ,

};
const Score mgRookTable [64] = {

      16,    2,   -3,   -1,   18,   44,   45,   68 ,
      42,   41,   50,   81,   62,  106,   82,  109 ,
      -1,   33,   29,   37,   89,  103,  130,  105 ,
      -2,    9,   11,   16,   16,   35,   50,   57 ,
     -23,  -27,  -20,  -14,  -16,  -18,   29,   25 ,
     -28,  -25,  -24,  -30,  -19,   -3,   64,   31 ,
     -58,  -52,  -28,  -34,  -20,   -5,   40,   -6 ,
      -9,   -8,   -8,   -2,    7,   15,   22,    1 ,

};
const Score mgQueenTable [64] = {

     -16,  -18,   15,   48,   52,   74,   80,   38 ,
      43,    6,   12,   25,   36,   82,   59,  118 ,
      20,   18,   10,   34,   51,  134,  115,  126 ,
       9,    2,   13,   11,    9,   41,   48,   58 ,
       5,   -2,    0,    6,   12,   11,   32,   54 ,
      17,   19,    9,   -2,   -5,   19,   54,   56 ,
      -8,  -12,   10,    6,    9,   17,   42,   70 ,
       5,   26,   26,   30,   26,    4,   26,   22 ,

};
const Score mgKingTable [64] = {

      15,   40,   44,    4,   19,   46,   79,   46 ,
      40,   93,   51,   96,   77,   99,  127,   89 ,
       5,   98,   45,   32,   67,  130,  114,   76 ,
      19,   26,    0,  -47,  -38,    8,   28,  -14 ,
       8,    9,  -31,  -77,  -71,  -27,  -28,  -62 ,
      43,   70,   -7,  -32,  -32,  -23,   35,   25 ,
     129,   63,   63,   12,   19,   28,  118,  135 ,
     130,  158,  151,   36,  121,   62,  154,  158 ,

};
const Score egPawnTable [64] = {

       0,   -5,   -2,   -1,    0,    0,   -4,   -5 ,
      43,   41,   39,   31,   34,   35,   36,   44 ,
      25,   26,   21,   14,   14,   17,   20,   20 ,
      15,   13,    3,   -5,   -7,   -1,    4,    4 ,
       7,    7,   -5,  -11,  -10,   -8,   -2,   -4 ,
     -17,  -21,  -23,  -21,  -17,  -15,  -21,  -24 ,
     -34,  -38,  -34,  -33,  -34,  -29,  -40,  -46 ,
       0,   -3,   -1,   -3,    2,   -1,    3,    0 ,

};
const Score egKnightTable [64] = {

     -60,  -14,   10,   -5,   -3,  -34,  -26,  -86 ,
      50,   56,   43,   34,   21,    7,   41,   15 ,
      33,   34,   61,   52,   27,   18,    9,    7 ,
      23,   30,   54,   54,   51,   45,   29,    7 ,
      24,   19,   47,   47,   54,   35,   14,   15 ,
     -19,  -12,   -5,   25,   20,   -6,  -17,  -10 ,
     -39,  -33,  -27,  -20,  -29,  -29,  -40,  -30 ,
      -9,   -9,    2,   12,   14,   -3,    1,   11 ,

};
const Score egBishopTable [64] = {

      22,   31,   24,   32,   23,   10,   15,    4 ,
      52,   57,   56,   49,   33,   42,   53,   48 ,
      56,   47,   47,   27,   27,   34,   35,   41 ,
      30,   34,   28,   43,   30,   31,   23,   25 ,
      12,   25,   35,   33,   31,   26,   20,   -5 ,
      -6,    2,    8,   17,   25,   12,   -7,  -13 ,
     -22,  -36,  -41,   -9,  -14,  -22,  -26,  -45 ,
       5,   18,   18,   16,   18,   30,    7,  -21 ,

};
const Score egRookTable [64] = {

      63,   66,   76,   68,   58,   57,   55,   47 ,
     106,  120,  116,   94,   95,   83,   90,   80 ,
      88,   86,   86,   73,   57,   50,   52,   42 ,
      71,   65,   74,   62,   47,   44,   44,   37 ,
      53,   54,   51,   44,   46,   44,   29,   25 ,
      24,   19,   15,   21,   12,    5,  -24,  -14 ,
      -5,   -4,   -3,    0,  -14,  -22,  -37,  -32 ,
      37,   29,   36,   24,   16,   22,   16,   18 ,

};
const Score egQueenTable [64] = {

      74,   86,  106,   98,  106,   90,   51,   77 ,
     122,  162,  181,  185,  202,  163,  150,  149 ,
     110,  117,  164,  166,  174,  150,  114,  121 ,
      94,  114,  122,  153,  174,  156,  154,  136 ,
      77,  102,  107,  136,  135,  134,  117,  111 ,
      27,   59,   73,   84,   98,   91,   66,   50 ,
      -4,    4,   13,   24,   26,    6,  -20,  -43 ,
      28,   25,   31,   46,   35,   36,   20,   14 ,

};
const Score egKingTable [64] = {

     -48,    7,   20,   49,   41,   53,   49,  -48 ,
      81,  126,  124,  111,  129,  144,  139,  103 ,
      75,  116,  132,  136,  143,  138,  134,   87 ,
      47,   92,  119,  130,  132,  127,  107,   69 ,
      22,   69,  100,  123,  121,  104,   85,   58 ,
      -3,   30,   59,   80,   81,   70,   43,   19 ,
     -45,   -7,   10,   30,   34,   24,   -6,  -45 ,
     -53,  -25,   -4,   20,   -3,   19,  -16,  -70 ,

};

extern Score* mgPestoTables[6];

extern Score* egPestoTables[6];

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };
extern Score mgTables[12][64];
extern Score egTables[12][64];

constexpr Score knightMobMg [9] = {-49, -11, 14, 30, 44, 55, 70, 83, 92, };
constexpr Score bishopMobMg [14] = {-65, -11, 10, 24, 42, 55, 63, 69, 72, 76, 88, 96, 102, 89, };
constexpr Score rookMobMg [15] = {-14, -12, 2, 3, 9, 6, 11, 19, 24, 38, 40, 43, 50, 59, 50, };
constexpr Score queenMobMg [28] = {-21, -117, -42, -12, 5, 14, 23, 30, 37, 43, 51, 57, 61, 68, 72, 79, 79, 80, 87, 105, 111, 136, 151, 157, 171, 164, 105, 88, };

constexpr Score knightMobEg [9] = {-68, 6, 40, 54, 65, 81, 79, 75, 60, };
constexpr Score bishopMobEg [14] = {-33, -23, 7, 32, 48, 63, 72, 76, 83, 83, 76, 74, 70, 63, };
constexpr Score rookMobEg [15] = {-88, 29, 44, 63, 72, 85, 93, 93, 100, 105, 110, 115, 119, 113, 117, };
constexpr Score queenMobEg [28] = {-11, -83, -64, -37, 9, 40, 82, 113, 135, 145, 155, 165, 176, 185, 194, 195, 213, 219, 220, 218, 221, 205, 196, 187, 180, 170, 137, 124, };
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