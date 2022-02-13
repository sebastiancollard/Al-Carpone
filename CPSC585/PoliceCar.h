#pragma once

#include "PxPhysicsAPI.h"
#include "Vehicle.h"
#include "Model.h"

/*
PoliceCar Entity Class
 - Used for containing Car Model, Transformation, and AI
*/

class PoliceCar : public Vehicle {
private:
	bool can_rob = false;				//If player "collides" with trigger capsule, this shoudl be set to true
	int cash = 0;						//Amount of cash the player has on-hand. Private variable with accessors & mutators?


public:

	// Call parent constructor
	PoliceCar() : Vehicle(VEHICLE_TYPE::POLICE_CAR, 1, physx::PxVec3(0, 0, 0)) {
		//TODO: Change ID?
	}


	// Must be called after graphics system is initalized!
	void createModel();
};