#include "constants.h"
#include "types.h"

int futilityMarginDelta = 79;       // [55, 125]
int nmpBias = 11;                   // [0, 30]
int nmpDepthDivisor = 3;            // [2, 6]
int nmpScoreDivisor = 248;          // [100, 300]
int nmpQ1 = 3;                      // [1, 4]
int nmpQ2 = 3;                      // [0, 4]
int razorQ1 = 255;                  // [50, 200]
int razorQ2 = 197;                  // [100, 300]
int singularDepthMultiplier = 4;    // [1, 6]
Depth IIRdepth = 4;                 // [3, 7]
Depth razorDepth = 3;               // [2, 5]
Depth singularDepth = 6;            // [4, 8]
Depth RFPDepth = 7;                 // [5, 10]
int futPruningMultiplier = 167;     // [50, 300]
int futPruningAdd = 244;            // [50, 300]