#pragma once
#include <queue>
#include <glm/glm.hpp>
#include <iostream>
#include "physx_globals.h"
#include "physXVehicleSettings.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include "Model.h"


#define CAR_MAX_VELOCITY_FORWARD 40.0f
#define CAR_MAX_VELOCITY_BACKWARD -11.35f

enum VEHICLE_TYPE
{
	AL_CARPONE = 0,
	POLICE_CAR
};

class Vehicle {
public:

	//void handle(State);

	bool vehicleInAir = true;
	bool vehicleChangingGears;
	unsigned int ID = -1;
	physx::PxRigidActor* actorPtr;	// Each vehicle instantiation has an actor (physx vehicle). Mostly used to query information about the car in the context of the simulation.
	physx::PxVehicleDrive4W* vehiclePtr;
	CarModel4W* car;

	std::queue<DriveMode> inputQueue;	// Input queue used to process multiple actions within a single update.

	// Constructors
	Vehicle() {}
	Vehicle(VEHICLE_TYPE type, unsigned int ID, PxTransform T);
	Vehicle(VEHICLE_TYPE type, unsigned int ID, physx::PxVec3 startOffset);

	// Transforms
	PxTransform getStartTransform();
	void moveStartPoint(PxVec3 v);


	// Fetch directions
	glm::vec3 getDir();		// fetch the front-facing direction of the player vehicle
	glm::vec3 getRight();	// fetch the vector pointing to the right of the vehicle
	glm::vec3 getUp();
	glm::vec3 getPos();		// fetch position of the vehicle chassis

	glm::vec3 getLinearVelocity();
	glm::vec3 getAngularVelocity();

	float getForwardVelocity();
	float getForwardAcceleration();
	float getForwardJerk();

	void updatePhysicsVariables(double);

	bool isMoving();
	bool isChangingGears();

	// Resets
	void setResetPoint(PxTransform t);
	void reset();

protected:
	

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 jerk;

	PxTransform startTransform;
	
};