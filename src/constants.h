#pragma once
#include "types.h"

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
#define LOGROOTMOVEDEPTH                12 // The depth at which the root moves are logged. Setting this to >= 128 will disable logging.

extern double lmrDepthValue;
extern double lmrMoveValue;
extern double lmrC;
extern double lmrA;
extern int futilityMarginDelta;
extern int nmpBias;
extern int nmpDepthDivisor;
extern int nmpScoreDivisor;
extern int nmpQ1;
extern int nmpQ2;
extern int razorQ1;
extern int razorQ2;
extern int singularDepthMultiplier;	 // [1, 6]
extern Depth IIRdepth;				 // [3, 7]
extern Depth razorDepth;			 // [2, 5]
extern Depth singularDepth;			 // [4, 8]
extern Depth RFPDepth;
extern int futPruningMultiplier; // [50, 300]
extern int futPruningAdd;		 // [50, 300]