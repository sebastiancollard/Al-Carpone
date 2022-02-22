#pragma once
#include "Ability.h"

enum UPGRADE_TYPE {		//NOTE: Have yet to include minimap upgrade and engine noise upgrade
	WEIGHT_CAPACITY,
	CAR_SPEED,
	ROB_SPEED
};

class Upgrade : public Ability {
private:
	UPGRADE_TYPE type;			//which type of upgrade

public:
	Upgrade(UPGRADE_TYPE power_type) {
		type = power_type;
	}
	void activate() {};
	void deactivate() {};

};