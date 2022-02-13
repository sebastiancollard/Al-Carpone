#pragma once

#include "PxPhysicsAPI.h"
#include "Vehicle.h"
#include "Model.h"







class PoliceCar : public Vehicle {
private:
	bool can_rob = false;				//If player "collides" with trigger capsule, this shoudl be set to true
	int cash = 0;						//Amount of cash the player has on-hand. Private variable with accessors & mutators?


public:

	PoliceCar() {}

	//Call parrent constructor
	PoliceCar(VEHICLE_TYPE type) : Vehicle(type, 0, physx::PxVec3(0, 0, 0)) {}


	// Must be called after graphics system is initalized!
	void createModel();
};