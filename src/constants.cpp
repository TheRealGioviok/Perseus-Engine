#include "constants.h"
#include "types.h"

S32 goodHistoryThreshold = 3640;    // [2000, 5000]
int futilityMarginDelta = 82;       // [55, 125]
int nmpBias = 15;                   // [0, 30]
int nmpDepthDivisor = 3;            // [2, 6]
int nmpScoreDivisor = 229;          // [100, 300]
int nmpQ1 = 3;                      // [1, 4]
int nmpQ2 = 3;                      // [0, 4]
int razorQ1 = 125;                  // [50, 200]
int razorQ2 = 225;                  // [100, 300]
int singularDepthMultiplier = 3;    // [1, 6]
Depth IIRdepth = 4;                 // [3, 7]
Depth razorDepth = 3;               // [2, 5]
Depth singularDepth = 5;            // [4, 8]
Depth RFPDepth = 7;                 // [5, 10]

