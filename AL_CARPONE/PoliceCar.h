#pragma once

#include "PxPhysicsAPI.h"
#include "Vehicle.h"
#include "Model.h"

/*
PoliceCar Entity Class
 - Used for containing Car Model, Transformation, and AI
*/

class PoliceCar : public Vehicle {

public:

	PoliceCar() {}

	// Call parent constructor
	PoliceCar(int ID) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(0, 0, 0)) {}

	PoliceCar(int ID, PxVec3 startOffset) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, startOffset) {}

	// Must be called after graphics system is initalized!
	void createModel();
};