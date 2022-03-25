#pragma once

#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include "physx_globals.h"
#include "Vehicle.h"
#include "Player.h"


#define AL_CARPONE_CHASSIS_MASS 1500.0f //original 1500.0f
#define AL_CARPONE_WHEEL_MASS 20.0f //original 20.0f
#define AL_CARPONE_CHASSIS_DIMS PxVec3(2.00344f, 1.15007f, 5.41695f)
#define AL_CARPONE_WHEEL_RADIUS 0.3005f
#define AL_CARPONE_WHEEL_WIDTH 0.295f


#define POLICE_CAR_CHASSIS_MASS 1500.0f //original 1500.0f
#define POLICE_CAR_WHEEL_MASS 20.0f //original 20.0f
#define POLICE_CAR_CHASSIS_DIMS PxVec3(2.07574f, 1.45513, 5.62142)
#define POLICE_CAR_WHEEL_RADIUS 0.30;//92494f
#define POLICE_CAR_WHEEL_WIDTH 0.347541f
 

//TODO move these to physics System

using namespace physx;
using namespace snippetvehicle;

VehicleDesc initVehicleDesc(VEHICLE_TYPE type)
{

	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	PxF32 chassisMass = AL_CARPONE_CHASSIS_MASS;
	if (type == POLICE_CAR) chassisMass = POLICE_CAR_CHASSIS_MASS;

	PxVec3 chassisDims = AL_CARPONE_CHASSIS_DIMS;	// current chassis mesh has these dimensions (inspected in blender)
	if (type == POLICE_CAR) chassisDims = POLICE_CAR_CHASSIS_DIMS;

	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);

	//const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.5f, 0.f);
	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.

	PxF32 wheelMass = AL_CARPONE_WHEEL_MASS;
	if (type == POLICE_CAR) wheelMass = POLICE_CAR_WHEEL_MASS;

	PxF32 wheelRadius = AL_CARPONE_WHEEL_RADIUS;	// current wheel mesh has diameter of 0.601m (inspected in blender)
	if (type == POLICE_CAR)  wheelRadius = POLICE_CAR_WHEEL_RADIUS;

	PxF32 wheelWidth = AL_CARPONE_WHEEL_WIDTH;	// and this width	(inspected in blender)
	if (type == POLICE_CAR) {
		wheelRadius = POLICE_CAR_WHEEL_WIDTH;
	}
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	PxMaterial* chassisMat = gPhysics->createMaterial(0.1f, 0.1f, 0.1f);

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = chassisMat;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
	if (type == POLICE_CAR)
		vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS_POLICE, COLLISION_FLAG_CHASSIS_POLICE_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);
	if (type == POLICE_CAR)
		vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL_POLICE, COLLISION_FLAG_WHEEL_POLICE_AGAINST, 0, 0);

	return vehicleDesc;
}


void startAccelerateForwardsMode(double speed)
{
	gVehicleInputData.setAnalogAccel(speed);
}

void startAccelerateReverseMode()
{
	gVehicleInputData.setAnalogAccel(1.0f);
}

void startBrakeMode()
{
	gVehicleInputData.setAnalogBrake(1.0f);
}

void startTurnHardLeftMode()
{
	gVehicleInputData.setAnalogSteer(0.5f);
}

void startTurnHardRightMode()
{
	gVehicleInputData.setAnalogSteer(-0.5f);
}

void startHandbrakeMode()
{
	gVehicleInputData.setAnalogHandbrake(0.5f);
}

void releaseAllControls()
{
	gVehicleInputData.setAnalogAccel(0.0f);
	gVehicleInputData.setAnalogSteer(0.0f);
	gVehicleInputData.setAnalogBrake(0.0f);
	gVehicleInputData.setAnalogHandbrake(0.0f);
}


void updateDrivingMode(Vehicle& vehicle)
{
	bool gasPedal = false;

	releaseAllControls();

	int queueSize = (int)vehicle.inputQueue.size();

	unsigned int previousGear = vehicle.vehiclePtr->mDriveDynData.getCurrentGear();

	for (int i = 0; i < queueSize; i++)
	{
		// get and pop first element in input queue
		DriveMode input = vehicle.inputQueue.front();
		vehicle.inputQueue.pop();

		// get total velocity of the car
		glm::vec3 velocity;
		{
			PxVec3 vel = vehicle.vehiclePtr->getRigidDynamicActor()->getLinearVelocity() + vehicle.vehiclePtr->getRigidDynamicActor()->getAngularVelocity();
			velocity = glm::vec3(vel[0], vel[1], vel[2]);
		}

		// lazy fix for enabling donuts
		if (input == eDRIVE_MODE_ACCEL_FORWARDS) {
			gasPedal = true;
		}
		if (glm::length(velocity) < 1.f && input == eDRIVE_MODE_HANDBRAKE && gasPedal) continue;

		// If we want to move forward but are currently in the reverse gear...
		if (eDRIVE_MODE_ACCEL_FORWARDS == input && vehicle.vehiclePtr->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE)
		{
			// naturally we would switch to first gear, but first we check if the car 
			// is going in a relatively opposite direction as its facing. If so, then we want the wheels to brake until we've reached a stop.
			if (glm::length(velocity) > 0 && glm::dot(glm::normalize(velocity), vehicle.getDir()) < 0) input = eDRIVE_MODE_BRAKE;
			else vehicle.vehiclePtr->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		}
		else if (eDRIVE_MODE_ACCEL_REVERSE == input && vehicle.vehiclePtr->mDriveDynData.getCurrentGear() != PxVehicleGearsData::eREVERSE) { // if we want to reverse but are currently not in the reverse gear...
			// then we want to change to the reverse gear, but first we check if the car is going 
			// relatively in the same direction as its facing. If so, then we want to brake until we've reached a stop. 
			if (glm::length(velocity) > 0 && glm::dot(glm::normalize(velocity), vehicle.getDir()) > 0) input = eDRIVE_MODE_BRAKE;
			else vehicle.vehiclePtr->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		}

		// Add changes from the corresponding input
		switch (input)
		{
		case eDRIVE_MODE_ACCEL_FORWARDS:
			startAccelerateForwardsMode(vehicle.speed);
			break;
		case eDRIVE_MODE_ACCEL_REVERSE:
			startAccelerateReverseMode();
			break;
		case eDRIVE_MODE_HARD_TURN_LEFT:
			startTurnHardLeftMode();
			break;
		case eDRIVE_MODE_HARD_TURN_RIGHT:
			startTurnHardRightMode();
			break;
		case eDRIVE_MODE_HANDBRAKE:
			startHandbrakeMode();
			break;
		case eDRIVE_MODE_BRAKE:
			startBrakeMode();
			break;
		case eDRIVE_MODE_NONE:
			break;
		}
	}

	if (vehicle.vehiclePtr->mDriveDynData.getCurrentGear() != previousGear) vehicle.vehicleChangingGears = true;
	else(vehicle.vehicleChangingGears = false);
}


