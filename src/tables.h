#pragma once

#include "types.h"
#include "constants.h"

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
extern Ply reductionTable[2][64][64];
extern S16 lmpMargin[128][2];

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
const Score kingSafety[] = {
	0, 0, 1, 2, 3, 5, 7, 9, 12, 15,
	18, 22, 26, 30, 35, 39, 44, 50, 56, 62,
	68, 75, 82, 85, 89, 97, 105, 113, 122, 131,
	140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
	260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
	377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
	494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500, 500, 500
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
