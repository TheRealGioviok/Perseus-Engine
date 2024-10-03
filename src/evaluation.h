#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {101, 380, 385, 556, 975, 0};
const Score mgPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      55,   15,   50,   77,   69,   41,  -54,  -51 ,
       7,   43,   65,   62,   74,   99,   66,    9 ,
       3,   -3,   14,   26,   36,   29,   -1,    2 ,
     -10,  -17,   -1,   16,   15,   12,  -11,   -8 ,
      -8,   -3,   -1,    7,   17,    2,    9,   -2 ,
      -9,   -4,   -5,   -6,   -3,    8,   13,  -12 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score mgKnightTable [64] = {

    -149, -101,  -75,   -3,   21,  -78, -110, -106 ,
       7,    6,   61,  122,   84,  118,   -1,   22 ,
      33,   73,   80,   92,  129,  103,   95,   40 ,
      41,   59,   77,   89,   83,  100,   77,   55 ,
      28,   51,   45,   54,   66,   54,   73,   46 ,
     -16,   10,   20,   29,   34,   30,   27,   10 ,
     -13,  -25,    2,   16,   20,    9,   -1,    9 ,
     -37,  -14,    1,   -3,    5,    3,   -5,  -30 ,

};
const Score mgBishopTable [64] = {

     -29,  -60,  -81, -103,  -82, -110,  -34,  -35 ,
     -21,   14,   17,    3,   32,   27,   15,  -12 ,
      30,   48,   46,   50,   54,   61,   65,   50 ,
      24,   44,   51,   69,   72,   55,   52,   28 ,
      23,   22,   33,   53,   58,   34,   31,   25 ,
      25,   43,   36,   42,   41,   41,   45,   41 ,
      40,   33,   34,   19,   23,   30,   50,   43 ,
      42,   33,   22,   13,   10,   19,   25,   43 ,

};
const Score mgRookTable [64] = {

      54,   34,   23,   30,   38,   41,   58,   77 ,
      33,   30,   52,   70,   65,   81,   51,   54 ,
      24,   79,   57,   77,  100,   91,  118,   64 ,
      20,   48,   52,   66,   64,   65,   74,   54 ,
     -12,   10,    8,    9,    5,    9,   28,   11 ,
     -24,   15,    1,   -1,    3,    7,   47,    7 ,
     -53,   -9,    4,    5,    8,   19,   27,  -33 ,
      -6,    5,   10,   16,   16,   20,   22,   13 ,

};
const Score mgQueenTable [64] = {

     152,  133,  139,  160,  155,  164,  146,  142 ,
     119,   70,  104,   88,  100,  140,   71,  115 ,
     143,  145,  125,  129,  135,  167,  189,  193 ,
     134,  144,  139,  123,  125,  146,  162,  166 ,
     126,  135,  120,  123,  130,  125,  144,  147 ,
     127,  137,  132,  126,  132,  135,  151,  144 ,
     120,  136,  143,  142,  138,  146,  141,  121 ,
     125,  140,  143,  145,  143,  143,  148,  131 ,

};
const Score mgKingTable [64] = {

      -2,   43,   35,   -1,   27,   35,   77,   10 ,
     -49,   26,    9,   74,   37,   72,   39,   38 ,
     -45,   80,   56,   24,   36,  108,   76,    7 ,
     -56,   12,    7,  -45,  -27,   11,   17,  -71 ,
     -60,   -2,  -24,  -78,  -78,  -36,  -33,  -94 ,
     -46,  -24,  -44,  -86,  -80,  -69,  -43,  -61 ,
      45,    0,  -29,  -91,  -82,  -62,    4,   36 ,
      33,   43,   11,  -52,   -5,  -51,   47,   51 ,

};
const Score knightMobMg [9] = {-21, -6, 8, 16, 24, 31, 39, 50, 60, };
const Score bishopMobMg [14] = {-53, -9, 16, 25, 36, 42, 48, 51, 56, 63, 77, 104, 119, 141, };
const Score rookMobMg [15] = {-14, -19, -7, -3, -1, -2, 3, 9, 13, 23, 27, 36, 41, 61, 97, };
const Score queenMobMg [28] = {-59, -23, 23, 43, 48, 55, 64, 69, 74, 81, 87, 93, 98, 101, 102, 105, 106, 104, 106, 115, 122, 133, 143, 172, 199, 222, 224, 217, };


const Score egValues [6] = {154, 455, 485, 816, 979, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      84,   89,   54,    0,   -1,   28,   88,  100 ,
      74,   67,   25,  -15,  -21,    5,   50,   66 ,
      38,   31,   10,  -12,  -15,   -2,   25,   27 ,
      16,   14,    1,  -10,  -12,   -2,    6,    8 ,
      10,    6,    9,    5,    6,   10,    2,    1 ,
      18,   19,   28,   27,   25,   27,   12,    8 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -38,   27,   61,   40,   38,   55,   42,  -62 ,
       8,   38,   35,   39,   48,   24,   33,    2 ,
      25,   34,   74,   72,   59,   67,   26,   25 ,
      36,   57,   80,   87,   89,   71,   60,   37 ,
      48,   49,   75,   81,   83,   75,   49,   42 ,
      17,   28,   55,   71,   72,   56,   31,   12 ,
      27,   36,   24,   42,   41,   24,   26,   29 ,
       1,   14,   14,   27,   32,   21,   19,  -10 ,

};
const Score egBishopTable [64] = {

      64,   66,   63,   71,   71,   61,   56,   58 ,
      47,   55,   57,   52,   42,   53,   55,   44 ,
      56,   55,   57,   48,   51,   58,   52,   51 ,
      48,   54,   53,   66,   65,   56,   52,   51 ,
      35,   47,   65,   63,   64,   61,   45,   35 ,
      34,   44,   54,   56,   58,   52,   40,   34 ,
      29,   22,   33,   48,   49,   38,   26,   24 ,
      27,   40,   48,   33,   39,   51,   41,   20 ,

};
const Score egRookTable [64] = {

      99,  107,  114,  109,  107,  109,  104,   95 ,
      95,  105,  102,   97,   99,   91,   98,   89 ,
      92,   74,   87,   77,   69,   75,   61,   80 ,
      86,   81,   85,   77,   76,   76,   76,   74 ,
      68,   77,   78,   76,   76,   79,   74,   63 ,
      47,   56,   57,   56,   55,   56,   44,   30 ,
      45,   45,   49,   46,   42,   37,   26,   32 ,
      45,   45,   48,   40,   38,   47,   39,   32 ,

};
const Score egQueenTable [64] = {

     343,  375,  398,  413,  419,  402,  383,  371 ,
     361,  412,  426,  463,  479,  430,  431,  390 ,
     362,  361,  421,  438,  435,  437,  370,  378 ,
     369,  391,  400,  435,  451,  423,  411,  383 ,
     341,  367,  381,  413,  403,  400,  365,  358 ,
     305,  318,  344,  343,  341,  354,  324,  305 ,
     283,  270,  270,  296,  299,  269,  269,  274 ,
     268,  260,  257,  281,  266,  244,  247,  243 ,

};
const Score egKingTable [64] = {

    -155,  -72,  -49,  -13,  -32,  -22,  -28, -158 ,
     -28,   37,   32,    8,   23,   38,   53,  -30 ,
      12,   51,   53,   52,   52,   51,   58,   14 ,
       7,   42,   60,   70,   68,   60,   49,   11 ,
     -18,   20,   48,   71,   69,   50,   29,   -6 ,
     -16,    4,   28,   51,   48,   33,    7,  -11 ,
     -39,  -13,    5,   22,   19,   14,  -16,  -45 ,
    -107,  -65,  -38,  -44,  -59,  -28,  -67, -121 ,

};
const Score knightMobEg [9] = {-94, -30, 7, 28, 38, 52, 50, 46, 26, };
const Score bishopMobEg [14] = {-150, -85, -30, -1, 18, 35, 47, 52, 60, 59, 54, 43, 47, 15, };
const Score rookMobEg [15] = {-84, -33, 16, 28, 49, 67, 77, 79, 85, 90, 94, 97, 102, 87, 71, };
const Score queenMobEg [28] = {-18, 5, 75, 162, 276, 338, 378, 401, 425, 434, 440, 450, 453, 462, 468, 472, 475, 481, 481, 478, 472, 460, 450, 431, 401, 378, 323, 320, };

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