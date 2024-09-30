#pragma once

#include "BBmacros.h"
#include "Position.h"

/**
 * @brief The simpleEval function evaluates a given board, taking into account the material value of the board.
 * @param board The board to evaluate.
 * @return The value of the board.
 * @note This evaluation function is not very good, but it is a good starting point.
 */

const Score mgValues [6] = {105, 398, 408, 580, 937, 0};
const Score mgPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      63,   21,   62,   80,   68,   50,  -52,  -46 ,
       3,   42,   60,   50,   58,   87,   60,    6 ,
       1,   -2,   12,   20,   29,   22,   -3,   -2 ,
      -8,  -14,    2,   17,   16,   14,  -10,   -7 ,
      -6,    0,    1,    8,   19,    4,   11,   -1 ,
      -8,   -2,   -3,   -4,   -1,   10,   15,  -11 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score mgKnightTable [64] = {

    -142,  -91,  -75,  -23,    1, -107, -110, -113 ,
       9,   10,   44,   98,   55,   88,   -9,   21 ,
      21,   64,   70,   67,   98,   76,   65,   13 ,
      37,   51,   54,   68,   59,   72,   46,   41 ,
      38,   45,   47,   51,   65,   43,   57,   45 ,
       2,   24,   35,   45,   49,   42,   37,   26 ,
       9,   -4,   17,   32,   35,   24,   14,   27 ,
     -21,    3,   17,   15,   21,   21,   10,  -13 ,

};
const Score mgBishopTable [64] = {

     -30,  -54,  -83, -115,  -95, -128,  -30,  -46 ,
     -30,   18,   23,   -7,   15,    9,    2,  -29 ,
      27,   48,   51,   39,   40,   49,   50,   31 ,
      21,   48,   42,   58,   63,   41,   45,   13 ,
      25,   26,   30,   53,   58,   36,   34,   23 ,
      26,   45,   44,   47,   47,   52,   51,   41 ,
      39,   41,   40,   29,   35,   40,   60,   46 ,
      45,   33,   28,   25,   20,   28,   27,   50 ,

};
const Score mgRookTable [64] = {

      42,   31,   14,    8,    7,   24,   53,   64 ,
      34,   26,   46,   56,   37,   58,   38,   50 ,
      31,   83,   58,   76,   89,   72,  104,   53 ,
      32,   62,   66,   80,   71,   70,   77,   53 ,
       3,   20,   19,   25,   18,   13,   30,   14 ,
      -7,   22,   12,   16,   19,   13,   43,   13 ,
     -30,   10,   20,   24,   24,   32,   36,  -21 ,
      12,   22,   28,   34,   32,   35,   35,   25 ,

};
const Score mgQueenTable [64] = {

      35,   48,   31,   25,   13,   36,   39,   30 ,
      40,   -6,   23,  -34,  -57,   18,  -21,   42 ,
      45,   38,   24,    9,    3,   21,   38,   47 ,
      50,   59,   44,   32,   23,   37,   56,   53 ,
      61,   62,   43,   51,   52,   47,   64,   64 ,
      63,   75,   68,   63,   67,   72,   81,   73 ,
      60,   78,   85,   85,   83,   91,   83,   59 ,
      68,   80,   84,   92,   88,   84,   86,   71 ,

};
const Score mgKingTable [64] = {

     -18,   34,   36,   10,   28,   29,   56,   -5 ,
     -54,   43,   54,  119,   82,  114,   53,   14 ,
     -33,  122,  124,   97,  110,  170,  111,   17 ,
     -43,   68,   97,   58,   82,  102,   73,  -63 ,
     -46,   68,   95,   59,   64,   91,   55,  -69 ,
     -42,   35,   63,   47,   56,   52,   33,  -48 ,
       4,    6,   15,  -29,  -19,  -10,   18,    2 ,
     -30,  -11,  -33,  -84,  -49,  -76,   -8,  -15 ,

};
const Score knightMobMg [9] = {-28, -15, -1, 8, 17, 24, 34, 45, 56, };
const Score bishopMobMg [14] = {-53, -13, 10, 18, 28, 33, 36, 37, 38, 44, 54, 78, 85, 105, };
const Score rookMobMg [15] = {-8, -13, -3, -1, 1, 1, 5, 11, 15, 23, 23, 30, 31, 48, 95, };
const Score queenMobMg [28] = {-95, -50, -8, 16, 20, 26, 34, 40, 45, 52, 58, 64, 69, 72, 73, 75, 76, 73, 75, 84, 90, 103, 100, 119, 106, 102, 86, 80, };

const Score egValues [6] = {163, 459, 482, 816, 908, 0};
const Score egPawnTable [64] = {

       0,    0,    0,    0,    0,    0,    0,    0 ,
      83,   94,   59,   12,   13,   38,   97,  101 ,
      80,   75,   37,    3,    0,   24,   63,   73 ,
      40,   35,   15,   -4,   -5,    9,   32,   30 ,
      13,   13,    1,  -10,  -11,   -1,    7,    6 ,
       6,    4,    7,    3,    3,    8,    0,   -3 ,
      14,   15,   24,   23,   21,   24,    8,    5 ,
       0,    0,    0,    0,    0,    0,    0,    0 ,

};
const Score egKnightTable [64] = {

     -40,   26,   63,   48,   47,   63,   43,  -60 ,
      11,   40,   42,   49,   60,   36,   40,    8 ,
      33,   39,   79,   82,   72,   76,   38,   36 ,
      45,   66,   92,  100,  103,   86,   78,   49 ,
      52,   58,   79,   87,   89,   83,   61,   50 ,
      20,   32,   56,   71,   72,   56,   35,   15 ,
      32,   41,   30,   45,   44,   29,   32,   35 ,
       7,   19,   20,   32,   37,   26,   22,   -3 ,

};
const Score egBishopTable [64] = {

      67,   67,   65,   72,   74,   65,   57,   61 ,
      57,   62,   59,   56,   48,   61,   65,   57 ,
      65,   63,   63,   55,   57,   65,   62,   65 ,
      57,   61,   61,   74,   72,   65,   60,   60 ,
      41,   52,   72,   70,   71,   67,   51,   41 ,
      39,   53,   61,   63,   65,   59,   47,   40 ,
      35,   28,   42,   56,   57,   46,   34,   29 ,
      30,   47,   56,   42,   49,   59,   49,   24 ,

};
const Score egRookTable [64] = {

     118,  122,  131,  128,  130,  127,  120,  114 ,
     111,  121,  119,  115,  122,  113,  116,  106 ,
     104,   86,   99,   90,   84,   92,   78,   96 ,
      97,   90,   95,   86,   86,   87,   87,   89 ,
      81,   92,   91,   87,   88,   94,   90,   79 ,
      61,   73,   72,   70,   68,   72,   64,   48 ,
      59,   59,   63,   60,   57,   52,   42,   47 ,
      60,   60,   63,   55,   53,   62,   54,   46 ,

};
const Score egQueenTable [64] = {

      91,   99,  127,  139,  148,  131,  111,  101 ,
     112,  162,  162,  202,  232,  166,  178,  123 ,
     123,  126,  158,  175,  169,  169,  133,  141 ,
     125,  145,  149,  176,  183,  158,  153,  129 ,
      94,  126,  140,  166,  154,  143,  114,  100 ,
      67,   85,  114,  110,  103,  115,   88,   57 ,
      55,   42,   43,   69,   73,   33,   36,   40 ,
      34,   31,   35,   64,   41,   15,   24,   14 ,

};
const Score egKingTable [64] = {

    -143,  -59,  -39,   -3,  -21,  -13,  -20, -148 ,
     -18,   43,   35,   10,   24,   37,   56,  -21 ,
      15,   51,   47,   45,   44,   44,   57,   13 ,
       6,   33,   44,   52,   47,   43,   37,    8 ,
     -23,    6,   25,   43,   40,   22,    8,  -16 ,
     -17,   -8,    6,   25,   21,    7,   -9,  -17 ,
     -22,   -7,    3,   15,   12,    9,  -11,  -29 ,
     -81,  -34,   -6,  -19,  -28,   -5,  -33,  -94 ,

};
const Score knightMobEg [9] = {-86, -21, 18, 40, 50, 64, 63, 59, 40, };
const Score bishopMobEg [14] = {-136, -71, -16, 12, 30, 47, 58, 63, 71, 70, 64, 51, 56, 22, };
const Score rookMobEg [15] = {-63, -12, 40, 51, 71, 90, 98, 99, 105, 109, 114, 116, 121, 106, 86, };
const Score queenMobEg [28] = {-132, -203, -141, -91, 5, 52, 82, 100, 117, 123, 128, 133, 134, 139, 142, 143, 144, 148, 146, 138, 131, 115, 113, 96, 97, 85, 88, 98, };

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