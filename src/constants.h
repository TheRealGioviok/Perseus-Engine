#pragma once
#include "types.h"

#include <vector>

// The search-uci communication frequency.

constexpr U64 comfrequency = 2047LL; // The search-uci communication frequency is expressed as 2^n - 1, and it will make the search communicate with the uci every 2^n nodes.

// The mate scores are used to recognize if a score is a mate or not.
constexpr Score infinity  = 32500;   // The score considered infinite.
constexpr Score mateScore = 32000;   // The score of mate in 0 ply.
constexpr Score mateValue = 31500;   // The score above which a score is to be considered a mate.
constexpr Score noScore  = -32500;   // The score considered no score.
constexpr Move  noMove   = 0;        // The move considered no move.

// The draw scores are used to recognize if a score is a draw or not.
constexpr Score drawScore = 0;       // The score of a draw.

// The score above which the position is theoretically won.
constexpr Score KNOWNWIN = 12000;   // The score above which the position is theoretically won.

// The maximum ply the engine will search to.
constexpr U16 maxPly = 128;        // The maximum ply the engine will search to.

// The number of colors and squares on the board.
constexpr int NUM_COLORS = 2;       // The number of colors.
constexpr int NUM_SQUARES = 64;     // The number of squares on the board.
constexpr int NUM_PIECES = 12;      // The number of piece types (color sensitive).
constexpr int NUM_PIECE_TYPES = 6;  // The number of piece types (color insensitive).

// ENUMS

// The enum of side to move.
enum Side {
	// White to move.
	WHITE = 0,
	// Black to move.
	BLACK = 1,
	// Unset or both to move.
	BOTH
};

// The enum of castle rights
enum CastleRights {
	// White castle kingside.
	WK = 1,
	// White castle queenside.
	WQ = 2,
	// Black castle kingside.
	BK = 4,
	// Black castle queenside.
	BQ = 8
};

// The enum of pieces
enum Pieces {
	P, N, B, R, Q, K, p, n, b, r, q, k, NOPIECE
};



// The enum of squares.
enum Squares {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1, noSquare
};

// FUNCTIONALITY SYMBOL DEFINITIONS

// SEARCH TECHNIQUES
/* -- TT PARAMETERS */
#define TTEXACTDEPTHBONUS 				0 // The depth bonus for exact entries in the transposition table.


/* -- SEARCH TECHNIQUES -- */
#define ENABLETTSCORING                 true // If true the transposition table is used for cutoffs. The transposition table is automatically implemented in the search function if this is true.
#define DETECTREPETITION                true // If true the repetition detection is used. The repetition detection is automatically implemented in the search function if this is true.
#define ENABLEPREFETCHING               true // If true the prefetching is used. The prefetching is automatically implemented in the search function if this is true.

/* -- PRUNING TECHNIQUES -- */
#define ENABLENMP                       true // If true the null move pruning is used. The null move pruning is automatically implemented in the search function if this is true.

/* -- MOVE ORDERING TECHNIQUES -- */
#define ENABLEMVVLVASORTING             true // If true the move ordering is sorted by the MVV/LVA heuristic. The move ordering is automatically implemented in the search function if this is true.
#define ENABLEPROMOTIONSORTING          true // If true the move ordering is sorted by the promotion heuristic. The move ordering is automatically implemented in the search function if this is true.
#define ENABLEPSQTDIFFSORTING           true // If true the move ordering is sorted by the difference in the piece square tables. The move ordering is automatically implemented in the search function if this is true.
#define ENABLEHISTORYHEURISTIC          true // If true the history heuristic is used. The history heuristic is automatically implemented in the search function if this is true.
#define ENABLETTORDERING                true // If true the transposition table is used for move ordering. The transposition table is automatically implemented in the search function if this is true. ENABLETTSCORING must be true for this to work.
#define ENABLEKILLERHEURISTIC           true // If true the killer heuristic is used. The killer heuristic is automatically implemented in the search function if this is true.
#define ENABLECOUNTERMOVEHEURISTIC      true // If true the counter move heuristic is used. The counter move heuristic is automatically implemented in the search function if this is true.
#define ENABLEBETTERHISTORYFORMULA      true // If true the better history formula is used. The better history formula is automatically implemented in the search function if this is true.

// QUIESCENCE TECHNIQUES
#define ENABLEQUIESCENCESEARCH          true // If true the quiescence search is used. The quiescence search is automatically implemented in the search function if this is true.
#define EVASIONSINQUIESCENCE            true // If true the search function will search for evasions in quiescence search. Quiescence search must be enabled for this to work.
#define DODELTAPRUNING                  true // If true the delta pruning is used. Quiescence search must be enabled for this to work.

// DEBUGGING SYMBOL DEFINITIONS
#define DEBUG                           false // If true the debugging is used. The debugging is automatically implemented in the code if this is true.
#define ASSERTS                         true // If true the asserts are used. The asserts are automatically implemented in the code if this is true.
#define LOGROOTMOVEDEPTH                127 // The depth at which the root moves are logged. Setting this to >= 128 will disable logging.

#define USE_PEXT                       // If defined, the BMI2 instruction set is used for magic bitboards. Make sure your CPU supports it!

struct TunableParam {
    std::string name;
    S32 defaultValue;
    S32 minValue;
    S32 maxValue;
	float cEnd;
	float rEnd;
	S32 value;

    inline TunableParam(std::string name, S32 defaultValue, S32 minValue, S32 maxValue, float cEnd, float rEnd)
        : name(name), defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), cEnd(cEnd), rEnd(rEnd), value(defaultValue){}
    
    inline operator int() const { return value; }
	inline TunableParam& operator=(int value) {
		this->value = value;
		return *this;
	}
};

std::vector<TunableParam>&  tunableParams();
TunableParam& addTune(std::string name, S32 defaultValue, S32 minValue, S32 maxValue, float cEnd, float rEnd);

//#define TUNE

#ifdef TUNE
	#define TUNE_PARAM(name, defaultValue, minValue, maxValue, cEnd, rEnd) \
		inline const TunableParam& tuned_##name = addTune(#name, defaultValue, minValue, maxValue, cEnd, rEnd); \
		inline S32 name() { return tuned_##name.value; }
#else
	#define TUNE_PARAM(name, defaultValue, minValue, maxValue, cEnd, rEnd); \
		constexpr S32 name() { return defaultValue; }
#endif

// Easy way to enable / disable some params from being modifiable
#define NO_TUNE_PARAM(name, defaultValue, minValue, maxValue, cEnd, rEnd); \
		constexpr S32 name() { return defaultValue; }

// History stuff
TUNE_PARAM(maxHistoryBonus, 2272, 1, 4096, 128, 0.002);
TUNE_PARAM(historyBonusQuadDepth, 9, 1, 32, 1, 0.002);
TUNE_PARAM(historyBonusLinDepth, 216, 1, 1024, 16, 0.002);
TUNE_PARAM(historyBonusOffset, -340, -1024, 1024, 52, 0.002);

TUNE_PARAM(maxHistoryMalus, 710, 1, 4096, 128, 0.002);
TUNE_PARAM(historyMalusQuadDepth, 4, 1, 32, 1, 0.002);
TUNE_PARAM(historyMalusLinDepth, 266, 1, 1024, 16, 0.002);
TUNE_PARAM(historyMalusOffset, 26, -1024, 1024, 52, 0.002);

// LMR table values
TUNE_PARAM(lmrDepthValue, 1011, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrMoveValue, 1128, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrA0, 697, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrC0, -324, -1000, 1000, 100, 0.002);
TUNE_PARAM(lmrA1, 752, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrC1, 763, 500, 1500, 50, 0.002);

// LMR additionals
TUNE_PARAM(lmrCieckA, 4401, 2048, 8192, 256, 0.002)
TUNE_PARAM(lmrCieckB, 1489, 500, 2500, 98, 0.002)
TUNE_PARAM(lmrCieckC, 741, 128, 1024, 48, 0.002)
TUNE_PARAM(lmrPV, 1169, 250, 2750, 125, 0.002)
TUNE_PARAM(lmrExpectedDecent, 694, 250, 2750, 125, 0.002)
TUNE_PARAM(lmrQuietHistoryDivisor, 8396, 2000, 14000, 500, 0.002)
TUNE_PARAM(lmrQuietCutNode, 1978, 250, 2750, 125, 0.002);
TUNE_PARAM(lmrQuietTTPV, 980, 250, 2750, 125, 0.002)
TUNE_PARAM(lmrBadNoisyCutNode, 648, 250, 2750, 125, 0.002);
TUNE_PARAM(lmrNoisyHistoryDivisorA, 5192, 2000, 14000, 500, 0.002)
TUNE_PARAM(lmrNoisyHistoryDivisorB, 6587, 2000, 14000, 500, 0.002)

// Deeper / shallower
TUNE_PARAM(doDeeperMargin, 33, 10, 50, 2, 0.002)

// PVS SEE
TUNE_PARAM(pvsSeeThresholdNoisy, -16, -160, -10, 6, 0.002)
TUNE_PARAM(pvsSeeThresholdQuiet, -109, -160, -10, 6, 0.002)
NO_TUNE_PARAM(pvsSeeMaxDepth, 7, 5, 10, 0.5, 0.002)

// LMP values
TUNE_PARAM(lmpA0, 597, 250, 1750, 64, 0.002);
TUNE_PARAM(lmpC0, 1929, 500, 5000, 198, 0.002);
TUNE_PARAM(lmpA1, 1023, 250, 1750, 64, 0.002);
TUNE_PARAM(lmpC1, 2795, 500, 5000, 198, 0.002);

// RFP values
TUNE_PARAM(futilityMarginDelta, 99, 45, 145, 4, 0.002);
NO_TUNE_PARAM(RFPDepth, 8, 5, 10, .5, 0.002);

// NMP values
TUNE_PARAM(nmpMarginBias, 177, 100, 300, 8, 0.002);
TUNE_PARAM(nmpDepthMargin, 26, 10, 50, 2, 0.002);
NO_TUNE_PARAM(nmpDepthDivisor, 4, 2, 6, .5, 0.002);
TUNE_PARAM(nmpScoreDivisor, 222, 100, 300, 8, 0.002);
NO_TUNE_PARAM(nmpQ1, 3, 1, 5, .5, 0.002);
NO_TUNE_PARAM(nmpQ2, 3, 0, 5, .5, 0.002);

// Razoring value
TUNE_PARAM(razorQ1, 97, -200, 200, 16, 0.002);
TUNE_PARAM(razorQ2, 239,  100, 500, 16, 0.002);
NO_TUNE_PARAM(razorDepth, 3, 2, 5, .5, 0.002);

// Singular Extensions
TUNE_PARAM(singularDepthMultiplier, 8, 1, 30, 1, 0.002);
TUNE_PARAM(doubleExtensionMargin, 21, 1, 50, 2, 0.002);
NO_TUNE_PARAM(singularSearchDepth, 7, 5, 10, .5, 0.002);

// History Pruning values
TUNE_PARAM(historyPruningMultiplier, -1508, -5120, -1024, 205, 0.002);
TUNE_PARAM(historyPruningBias, 157, -2048, 2048, 205, 0.002);

// IIR values
NO_TUNE_PARAM(IIRDepth, 6, 3, 8, .5, 0.002);

// FFP values
TUNE_PARAM(futPruningMultiplier, 49, 30, 130, 4, 0.002);
TUNE_PARAM(futPruningAdd, 254, 150, 350, 8, 0.002);
NO_TUNE_PARAM(futPruningDepth, 9, 6, 10, 0.5, 0.002);
TUNE_PARAM(qsFutilityMargin, 77, 0, 150, 8, 0.002)

// Move ordering values
TUNE_PARAM(captScoreMvvMultiplier, 14, 8, 32, 1, 0.002)

// Time management values
TUNE_PARAM(timeTmA, 141, 10, 200, 2, 0.002)
TUNE_PARAM(timeTmB, 989, 0, 1000, 13, 0.002)
TUNE_PARAM(timeTmOptimScale, 420, 100, 1000, 11, 0.002)
TUNE_PARAM(nodesTmMax, 1867, 1500, 2500, 13, 0.002)
TUNE_PARAM(nodesTmMul, 1359, 750, 1500, 10, 0.002)

// CorrHist scale values
TUNE_PARAM(MAXCORRHISTUNSCALED, 39, 16, 128, 8, 0.002)
TUNE_PARAM(MAXCORRHISTMILLIUPDATE, 193, 128, 512, 18, 0.002)

// CorrHist correction values
TUNE_PARAM(pawnCorrWeight, 1310, 0, 2048, 102, 0.002)
TUNE_PARAM(nonPawnCorrWeight, 1128, 0, 2048, 102, 0.002)
TUNE_PARAM(T0CorrWeight, 1160, 0, 2048, 102, 0.002)
TUNE_PARAM(T1CorrWeight, 885, 0, 2048, 102, 0.002)
TUNE_PARAM(T2CorrWeight, 1003, 0, 2048, 102, 0.002)
TUNE_PARAM(T3CorrWeight, 1125, 0, 2048, 102, 0.002)
TUNE_PARAM(T4CorrWeight, 1230, 0, 2048, 102, 0.002)
TUNE_PARAM(T5CorrWeight, 873, 0, 2048, 102, 0.002)
TUNE_PARAM(T6CorrWeight, 1116, 0, 2048, 102, 0.002)
TUNE_PARAM(T7CorrWeight, 934, 0, 2048, 102, 0.002)
TUNE_PARAM(T8CorrWeight, 1013, 0, 2048, 102, 0.002)
TUNE_PARAM(T9CorrWeight, 1173, 0, 2048, 102, 0.002)

// CorrHist update values
TUNE_PARAM(pawnCorrUpdate, 1061, 0, 2048, 102, 0.002)
TUNE_PARAM(nonPawnCorrUpdate, 1054, 0, 2048, 102, 0.002)
TUNE_PARAM(T0CorrUpdate, 828, 0, 2048, 102, 0.002)
TUNE_PARAM(T1CorrUpdate, 877, 0, 2048, 102, 0.002)
TUNE_PARAM(T2CorrUpdate, 1016, 0, 2048, 102, 0.002)
TUNE_PARAM(T3CorrUpdate, 1047, 0, 2048, 102, 0.002)
TUNE_PARAM(T4CorrUpdate, 1124, 0, 2048, 102, 0.002)
TUNE_PARAM(T5CorrUpdate, 1042, 0, 2048, 102, 0.002)
TUNE_PARAM(T6CorrUpdate, 903, 0, 2048, 102, 0.002)
TUNE_PARAM(T7CorrUpdate, 1106, 0, 2048, 102, 0.002)
TUNE_PARAM(T8CorrUpdate, 1248, 0, 2048, 102, 0.002)
TUNE_PARAM(T9CorrUpdate, 924, 0, 2048, 102, 0.002)
