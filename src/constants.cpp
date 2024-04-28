#include "constants.h"
#include "types.h"

int futilityMarginDelta = 87;       // [55, 125]
int nmpBias = 12;                   // [0, 30]
int nmpDepthDivisor = 3;            // [2, 6]
int nmpScoreDivisor = 243;          // [100, 300]
int nmpQ1 = 3;                      // [1, 4]
int nmpQ2 = 3;                      // [0, 4]
int razorQ1 = 241;                  // [50, 200]
int razorQ2 = 207;                  // [100, 300]
int singularDepthMultiplier = 4;    // [1, 6]
Depth IIRdepth = 5;                 // [3, 7]
Depth razorDepth = 3;               // [2, 5]
Depth singularDepth = 6;            // [4, 8]
Depth RFPDepth = 7;                 // [5, 10]
int futPruningMultiplier = 168;     // [50, 300]
int futPruningAdd = 239;            // [50, 300]