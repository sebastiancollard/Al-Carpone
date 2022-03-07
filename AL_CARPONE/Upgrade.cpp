#include "Upgrade.h"
#include<tuple>

Upgrade::Upgrade(uint upgradeType, uint upgradeSpecifier, uint highestTier) {
	maxTier = highestTier;
	tier = 0;

	type = upgradeType;
	specifier = upgradeSpecifier;

	std::tuple<float, bool, uint> tierInfo = std::make_tuple(0.f, false, 0);
	for (uint i = 0; i < highestTier; ++i) {
		upgradeInfo.push_back(tierInfo);
	}
}

// get reference for amount the current upgrade tier would add to the target parameter. The way the value would change the parameter depends if its a multiplier or an addition.
float& Upgrade::delta(uint tier) {
	return std::get<0>(this->upgradeInfo[tier]);
}

// get reference for whether the target should be multiplied by the delta or added together.
bool& Upgrade::isMultiplier(uint tier) {
	return std::get<1>(this->upgradeInfo[tier]);
}

// get reference for the cost of the current tier of the upgrade
uint& Upgrade::cost(uint tier) {
	return std::get<2>(this->upgradeInfo[tier]);
}