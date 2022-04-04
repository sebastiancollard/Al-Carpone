#pragma once
#include <vector>
#include "Model.h"

typedef unsigned int uint;

// total list of upgrades go here
enum UPGRADE_SPECIFIER {
	ROB_SPEED = 0,
	ALARM_CHANCE,
	DETECTION_RADIUS,
	MINIMAP,

	TOP_SPEED,
	GEAR_SPEED,

	TIRE_FRICTION,
	TURN_RADIUS,
	CAR_FLIP
};

// we'll wanna have more types, these are just ones we'll want for sure. used mostly to determine which garage each upgrade will belong to.
enum UPGRADE_TYPE {
	ROBBERY = 0,
	ENGINE,
	HANDLING
};

class Upgrade {
public:
	uint maxTier;
	uint tier;

	uint type;
	uint specifier;

	// stores three values per upgrade tier. 1: delta that affects the target parameter 2: whether the delta is multiplicative or additive 3: cost for the given tier
	std::vector<std::tuple<float, bool, uint>> upgradeInfo;
	
	std::vector<Model> menuElements;

	Upgrade(uint upgradeType, uint upgradeSpecifier, uint highestTier);

	// functions that double up as both getters and setters
	float& delta(uint tier);
	bool& isMultiplier(uint tier);
	uint& cost(uint tier);
};