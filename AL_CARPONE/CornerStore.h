#pragma once

#include "Building.h"

class CornerStore : public Building {

	PxVec3 tpos;

public:

	CornerStore(physx::PxVec3 pos) {
		
		tpos = pos;

		createTrigger();
	}

	void createTrigger(); // Must only be called once!
	void triggerFunction(Player& player, State& state);
};