#pragma once
#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleUtil.h"

using namespace physx;

extern PxF32 gSteerVsForwardSpeedData[2 * 8];
extern PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable;

extern PxVehicleKeySmoothingData gKeySmoothingData;

extern PxVehiclePadSmoothingData gPadSmoothingData;

extern PxVehicleDrive4WRawInputData gVehicleInputData;

enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE,
};