#include "constants.h"
#include "types.h"
#include <string>

std::vector<TunableParam>&  tunableParams() {
    static std::vector<TunableParam> tunableParams{};
    return tunableParams;
}

TunableParam& addTune(std::string name, S32 defaultValue, S32 minValue, S32 maxValue, float cEnd, float rEnd) {
    tunableParams().emplace_back(name, defaultValue, minValue, maxValue, cEnd, rEnd);
	return tunableParams().back();
}


// double lmrDepthValue = 1000;
double lmrMoveValue = 1000;
double lmrA0 = 763;
double lmrC0 = -275;
double lmrA1 = 775;
double lmrC1 = 625;
int lmpA0 = 500;
int lmpC0 = 1500;
int lmpA1 = 1000;
int lmpC1 = 3000;

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
int futPruningMultiplier = 60;     // [50, 300]
int futPruningAdd = 250;            // [50, 300]
int captScoreMvvMultiplier = 16;    // [8, 32]