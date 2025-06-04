#pragma once

#include "types.h"
#include "constants.h"
#include <array>
#define RESOLUTION 1000

//tropism
extern int distBonus[64][64];
extern int qkdist[64][64];
extern int rkdist[64][64];
extern int nkdist[64][64];
extern int bkdist[64][64];
extern int kbdist[64][64];
// mhDist
extern U8 manhattanDistance[64][64];
// cbyDist
extern U8 chebyshevDistance[64][64];
// kingshelter
extern BitBoard kingShelter[2][64];
// flank
extern BitBoard kingFlank[2][8];

// centerDistance
extern U8 centerDistance[64];
// squaresbetween
extern BitBoard squaresBetween[64][64];
// pv table and pv length
extern Move pvTable[maxPly][maxPly];
extern Ply pvLen[maxPly];
// Repetition Table
extern HashKey repetitionTable[512];
// The LMR reduction table
extern S32 reductionTable[2][64][64];
extern S32 lmpMargin[128][2];


constexpr S32 KSTABLESIZE = 256;
constexpr S32 MIN_MGX = -1024;
constexpr S32 MAX_MGX = 3517;
constexpr S32 MIN_EGX = -5238;
constexpr S32 MAX_EGX = 1024;
extern std::array<S32, KSTABLESIZE> kingSafetyMgTable;
extern std::array<S32, KSTABLESIZE> kingSafetyEgTable;


/**
 * @brief The initLMRTable function initializes the LMR reduction table
 */
void initLMRTable();

constexpr S8 colorCornerDistance[2][64] = {
	{
		+9, +4, -1, +0, -4, -6, -8, -9,
		+4, -1, -3, -7, -7, -8, -9, -8,
		+1, -3, -7, -7, -8, -9, -8, -6,
		+0, -7, -7, -10, -10, -8, -7, -4,
		-4, -7, -8, -10, -10, -7, -7, +0,
		-6, -8, -9, -8, -7, -7, -3, +1,
		-8, -9, -8, -7, -7, -3, -1, +4,
		-9, -8, -6, -4, +0, +1, +4, +9,
	},
	{
		-9, -8, -6, -4, +0, +1, +4, +9,
		-8, -9, -8, -7, -7, -3, -1, +4,
		-6, -8, -9, -8, -7, -7, -3, +1,
		-4, -7, -8, -10, -10, -7, -7, +0,
		+0, -7, -7, -10, -10, -8, -7, -4,
		+1, -3, -7, -7, -8, -9, -8, -6,
		+4, -1, -3, -7, -7, -8, -9, -8,
		+9, +4, -1, +0, -4, -6, -8, -9,
	}
};

constexpr BitBoard boardSide[2] = {
	1085102592571150095ULL, 
	17361641481138401520ULL
};

constexpr U64 MvvLva[12][12] = {
	// Aggressor - Victim
	{105, 205, 305, 405, 505, 605,	105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604,	104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603,	103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602,	102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601,	101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600,	100, 200, 300, 400, 500, 600},

	{105, 205, 305, 405, 505, 605,	105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604,	104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603,	103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602,	102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601,	101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600,	100, 200, 300, 400, 500, 600},
};

constexpr U32 promotionBonus[12] = {
	0, 70000, 80000, 90000, 100000, 0, 0, 70000, 80000, 90000, 100000, 0};

// Thanks to loki for the following 2 tables!
// Indexed by 7 + rank - file
// The diagonals start at the h1-h1 and end in the a8-a8
constexpr BitBoard diagonalMasks[15] = {
		128ULL,
		32832ULL,
		8405024ULL,
		2151686160ULL,
		550831656968ULL,
		141012904183812ULL,
		36099303471055874ULL,
		9241421688590303745ULL,
		4620710844295151872ULL,
		2310355422147575808ULL,
		1155177711073755136ULL,
		577588855528488960ULL,
		288794425616760832ULL,
		144396663052566528ULL,
		72057594037927936ULL
};

// Indexed by: rank + file
// Anti-diagonals go from a1-a1 to h8-h8
constexpr BitBoard antidiagonalMasks[15] = {
	1ULL,
	258ULL,
	66052ULL,
	16909320ULL,
	4328785936ULL,
	1108169199648ULL,
	283691315109952ULL,
	72624976668147840ULL,
	145249953336295424ULL,
	290499906672525312ULL,
	580999813328273408ULL,
	1161999622361579520ULL,
	2323998145211531264ULL,
	4647714815446351872ULL,
	9223372036854775808ULL
};

extern const U64 MvvLva[12][12];

extern const U32 promotionBonus[12];

extern BitBoard squaresToRight[64];
extern BitBoard squaresToLeft[64];
extern BitBoard passedPawnMask[2][64];
extern BitBoard isolatedPawnMask[64];
extern BitBoard backwardPawnMask[2][64];
extern BitBoard phalanx[64];
extern BitBoard alignedSquares[64][64];
const PScore kingSafety[64] = { // TODO: Replace with PScore tuned values
	S( 10 , 15 ), S( 12 , 17 ), S( 13 , 19 ), S( 15 , 22 ), S( 17 , 25 ), S( 19 , 28 ), S( 21 , 31 ), S( 24 , 35 ), 
	S( 27 , 40 ), S( 31 , 45 ), S( 34 , 51 ), S( 39 , 57 ), S( 43 , 64 ), S( 49 , 72 ), S( 55 , 80 ), S( 61 , 90 ), 
	S( 68 , 100 ), S( 76 , 112 ), S( 85 , 125 ), S( 94 , 138 ), S( 105 , 153 ), S( 116 , 170 ), S( 128 , 187 ), S( 140 , 206 ), 
	S( 154 , 226 ), S( 169 , 247 ), S( 184 , 270 ), S( 200 , 293 ), S( 216 , 318 ), S( 233 , 343 ), S( 251 , 368 ), S( 269 , 394 ), 
	S( 286 , 420 ), S( 304 , 447 ), S( 322 , 473 ), S( 340 , 498 ), S( 357 , 523 ), S( 373 , 548 ), S( 389 , 571 ), S( 404 , 594 ), 
	S( 419 , 615 ), S( 433 , 635 ), S( 445 , 654 ), S( 457 , 671 ), S( 468 , 688 ), S( 479 , 703 ), S( 488 , 716 ), S( 497 , 729 ), 
	S( 505 , 741 ), S( 512 , 751 ), S( 518 , 761 ), S( 524 , 769 ), S( 530 , 777 ), S( 534 , 784 ), S( 539 , 790 ), S( 542 , 796 ), 
	S( 546 , 801 ), S( 549 , 806 ), S( 552 , 810 ), S( 554 , 813 ), S( 556 , 816 ), S( 558 , 819 ), S( 560 , 822 ), S( 561 , 824 ), 
};

constexpr Score connectedSeed[] = { 0, 5, 6, 9, 23, 37, 67 };
constexpr Score blockBonusMg[] = { 0, -9, -2 };
constexpr Score blockBonusEg[] = { 0, -3, 3 };

extern BitBoard outerRing[64]; // The ring of squares in a centered 5x5 square
extern BitBoard fiveSquare[64]; // The 5x5 square
extern BitBoard kingShelter[2][64]; // The king shelter

/**
 * @brief The initEvalTables function initializes the evaluation tables
 */
void initEvalTables();

/**
 * @brief The initAll function initializes the tables.
 * @note THis is called at the beginning of the program.
 */
void initAll();
