#pragma once

#include "Building.h"

class Exit : public Building {
	physx::PxVec3 b_pos = physx::PxVec3(99, 20, -720);	// Position currently hardcoded

	float depth = 17.f;	 //y axis
	float height = 17.f; //z axis
	float width = 17.f;	 //x axis

	Model gameWinScreen;
	Model press_f_to_exit;

public:

	Exit() { 
		createTrigger();
		press_f_to_exit = Model("models/popups/press_f_to_exit.obj");
	}

	void createTrigger(); // Must only be called once!
	void triggerFunction(Player& player, State& state); 
};