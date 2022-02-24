#include "Upgrade.h"

Upgrade::Upgrade(UPGRADE_TYPE power_type, Player p) {
	type = power_type;
	player = p;
}


void Upgrade::activate() {
	isActive = true;
	switch (type) {
	case WEIGHT_CAPACITY:
		//increase weight capacity of player
		break;
	case CAR_SPEED:
		//increase speed of player
		break;
	case ROB_SPEED:
		//increase player rob speed
		break;
	}
}

void Upgrade::deactivate() {
	isActive = false;
	switch (type) {
	case WEIGHT_CAPACITY:
		//un-apply weight capacity increase
		break;
	case CAR_SPEED:
		//un-apply car speed increase
		break;
	case ROB_SPEED:
		//un-apply rob speed increase
		break;
	}
}

