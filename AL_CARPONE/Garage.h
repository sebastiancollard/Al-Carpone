#pragma once
#include<vector>
#include<glm/glm.hpp>
#include "Upgrade.h"
#include "Building.h"

class Garage : public Building {
public:
	unsigned int type;
	std::vector<Upgrade> upgradeList;
	physx::PxVec3 position = physx::PxVec3(250, 0, -105);	// Position currently hardcoded

	float depth = 21.6f;	 //y axis
	float height = 4.f; //z axis
	float width = 21.6;	 //x axis

	Garage(unsigned int upgradeType) { createTrigger(); }

	void createTrigger(); // Must only be called once!
	void triggerFunction(Player& player, State& state); // ENTER GARAGE
};