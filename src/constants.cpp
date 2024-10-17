#include "constants.h"
#include "types.h"

int futilityMarginDelta = 91;       // [55, 125]
int nmpDepthDivisor = 3;            // [2, 6]
int nmpScoreDivisor = 200;          // [100, 300]
int nmpQ1 = 3;                      // [1, 4]
int nmpQ2 = 3;                      // [0, 4]
int razorQ1 = 128;                  // [50, 200]
int razorQ2 = 192;                  // [100, 300]
int singularDepthMultiplier = 1;    // [1, 6]
int maximumDoubleExtensions = 6;
int doubleExtensionMargin = 24;
Depth IIRdepth = 4;                 // [3, 7]
Depth razorDepth = 4;               // [2, 5]
Depth singularSearchDepth = 7;            // [4, 8]
Depth RFPDepth = 7;                 // [5, 10]
int futPruningMultiplier = 162;     // [50, 300]
int futPruningAdd = 240;            // [50, 300]
int captScoreMvvMultiplier = 16;    // [8, 32]