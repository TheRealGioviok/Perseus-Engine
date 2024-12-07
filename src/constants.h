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

// #define TUNE

#ifdef TUNE
	#define TUNE_PARAM(name, defaultValue, minValue, maxValue, cEnd, rEnd) \
		inline const TunableParam& tuned_##name = addTune(#name, defaultValue, minValue, maxValue, cEnd, rEnd); \
		inline S32 name() { return tuned_##name.value; }
#else
	#define TUNE_PARAM(name, defaultValue, minValue, maxValue, cEnd, rEnd); \
		constexpr S32 name() { return defaultValue; }
#endif

// LMR table values
TUNE_PARAM(lmrDepthValue, 922, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrMoveValue, 1080, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrA0, 791, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrC0, -267, -1000, 1000, 100, 0.002);
TUNE_PARAM(lmrA1, 750, 500, 1500, 50, 0.002);
TUNE_PARAM(lmrC1, 619, 500, 1500, 50, 0.002);
// LMR additionals
TUNE_PARAM(lmrCieckA, 4258, 2048, 8192, 308, 0.002)
TUNE_PARAM(lmrCieckB, 1558, 500, 2500, 100, 0.002)
TUNE_PARAM(lmrCieckC, 652, 128, 1024, 45, 0.002)
TUNE_PARAM(lmrPV, 1029, 250, 1750, 75, 0.002)
TUNE_PARAM(lmrExpectedDecent, 755, 250, 2750, 150, 0.002)
TUNE_PARAM(lmrQuietHistoryDivisor, 7991, 2000, 16000, 700, 0.002)
TUNE_PARAM(lmrQuietCutNode, 1896, 250, 2750, 150, 0.002);
TUNE_PARAM(lmrQuietTTPV, 919, 250, 2750, 150, 0.002)
TUNE_PARAM(lmrBadNoisyCutNode, 766, 250, 2750, 150, 0.002);
TUNE_PARAM(lmrNoisyHistoryDivisorA, 5338, 2000, 16000, 700, 0.002)
TUNE_PARAM(lmrNoisyHistoryDivisorB, 6521, 2000, 16000, 700, 0.002)

// LMP values
TUNE_PARAM(lmpA0, 528, 500, 1500, 50, 0.002);
TUNE_PARAM(lmpC0, 1576, 500, 5000, 225, 0.002);
TUNE_PARAM(lmpA1, 978, 500, 1500, 50, 0.002);
TUNE_PARAM(lmpC1, 2858, 500, 5000, 225, 0.002);

// RFP values
TUNE_PARAM(futilityMarginDelta, 91, 55, 155, 5, 0.002);
TUNE_PARAM(RFPDepth, 8, 5, 10, .5, 0.002);

// NMP values
TUNE_PARAM(nmpDepthDivisor, 4, 2, 6, .5, 0.002);
TUNE_PARAM(nmpScoreDivisor, 213, 100, 300, 10, 0.002);
TUNE_PARAM(nmpQ1, 3, 1, 5, .5, 0.002);
TUNE_PARAM(nmpQ2, 3, 0, 5, .5, 0.002);

// Razoring value
TUNE_PARAM(razorQ1, 127, -200, 200, 20, 0.002);
TUNE_PARAM(razorQ2, 232,  100, 500, 20, 0.002);
TUNE_PARAM(razorDepth, 3, 2, 5, .5, 0.002);

// Singular Extensions
TUNE_PARAM(singularDepthMultiplier, 10, 1, 30, 2, 0.002);
TUNE_PARAM(maximumDoubleExtensions, 6, 1, 9, 1, 0.002);
TUNE_PARAM(doubleExtensionMargin, 23, 1, 50, 2.5, 0.002);
TUNE_PARAM(singularSearchDepth, 7, 5, 10, .5, 0.002);

// History Pruning values
TUNE_PARAM(historyPruningMultiplier, -1422, -5120, -1024, 205, 0.002);
TUNE_PARAM(historyPruningBias, 0, -6, 2048, 205, 0.002);

// IIR values
TUNE_PARAM(IIRDepth, 5, 3, 8, .5, 0.002);

// FFP values
TUNE_PARAM(futPruningMultiplier, 56, 30, 130, 5, 0.002);
TUNE_PARAM(futPruningAdd, 246, 150, 350, 10, 0.002);
TUNE_PARAM(futPruningDepth, 9, 6, 10, 0.5, 0.002);

// Move ordering values
TUNE_PARAM(captScoreMvvMultiplier, 14, 8, 32, 1, 0.002)

// Time management values
TUNE_PARAM(timeTmA, 135, 10, 200, 9, 0.002)
TUNE_PARAM(timeTmB, 988, 0, 1000, 50, 0.002)
TUNE_PARAM(timeTmOptimScale, 340, 100, 1000, 45, 0.002)
TUNE_PARAM(nodesTmMax, 1895, 1000, 3000, 100, 0.002)
TUNE_PARAM(nodesTmMul, 1309, 750, 1500, 38, 0.002)