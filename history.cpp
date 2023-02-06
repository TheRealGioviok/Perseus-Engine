#include "history.h"
#include "types.h"

// history table
S32 historyTable[2][64][64];
S32 pieceToHistoryTable[12][64];   // Indexed by Piece - To
S32 pieceFromHistoryTable[12][64]; // Indexed by Piece - From

// killer table
Move killerTable[2][maxPly];

// counter move table
Move counterMoveTable[NUM_PIECES][NUM_SQUARES];