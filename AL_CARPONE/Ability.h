#pragma once
#include "Player.h"

//enum duration { INF, ONE_M, THIRTY_S, FIFTEEN_S};

class Ability {
protected:
	bool isActive;				//is the ability currently being used by the player / currently activated.
	Player player;

public:
	bool isVisible;				//should the object be rendered (mostly thinking of if we have "pick-ups" if we don't get to robbing corner stores (which should be straightforward));

	//common functions between upgrades and power-ups
	virtual void activate();
	virtual void deactivate();
};