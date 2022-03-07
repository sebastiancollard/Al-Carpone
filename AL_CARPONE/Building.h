#pragma once

#include "BoxTrigger.h"
#include "Player.h"
#include "State.h"

// For compiler not to complain
class Player;
class State;


// Any building with a trigger
class Building {
public:
	BoxTrigger* trigger;
	
	bool isInRange = false;		// colliding with trigger 

	virtual void createTrigger() {
		std::cout << "BUILDING: createTrigger() not overriding" << std::endl;
	}

	// Function to execute when colliding with trigger (rob, get item, etc.)
	virtual void triggerFunction(Player& player, State& state){
		std::cout << "BUILDING: triggerFunction() not overriding" << std::endl;
	} 
};