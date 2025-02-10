#include "constants.h"
#include "types.h"
#include <string>

std::vector<TunableParam>&  tunableParams() {
    static std::vector<TunableParam> tunableParams{};
    tunableParams.reserve(128);
    return tunableParams;
}

TunableParam& addTune(std::string name, S32 defaultValue, S32 minValue, S32 maxValue, float cEnd, float rEnd) {
    tunableParams().emplace_back(name, defaultValue, minValue, maxValue, cEnd, rEnd);
	return tunableParams().back();
}