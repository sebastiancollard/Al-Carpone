#pragma once
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "PxPhysicsAPI.h"
#include <vector>
#include "Model.h"
#include "PowerUp.h"


using namespace physx;


//Set up physx global variables
//Should be checked over to see what actually needs to be global and what doesnt

#define PLAYER_BASE_ENGINE_PEAK_TORQUE 400
#define PLAYER_BASE_GEAR_CHANGE_SPEED .75
#define PLAYER_BASE_STEER_ANGLE PxPi * 0.25
static unsigned int timesUpgradedFriction = 0;

extern PxDefaultAllocator gAllocator;
extern PxDefaultErrorCallback gErrorCallback;

extern PxFoundation* gFoundation;
extern PxPhysics* gPhysics;

extern PxDefaultCpuDispatcher* gDispatcher;
extern PxScene* gScene;


extern PxCooking* gCooking;

extern PxMaterial* gMaterial;

extern PxPvd* gPvd;

struct physx_actor_entity
{
	PxRigidActor* actorPtr;
	PxU32 actorId;
};
extern std::vector<physx_actor_entity> physx_actors;
extern std::vector<PowerUp> active_items;

extern snippetvehicle::VehicleSceneQueryData* gVehicleSceneQueryData;
extern PxBatchQuery* gBatchQuery;

extern PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs;

extern PxRigidStatic* gGroundPlane;
extern PxRigidStatic* garageDoor;
extern bool garageDoorOpen;
extern bool garageDoorPrev;

extern PxActor* activeLevelActorPtr;