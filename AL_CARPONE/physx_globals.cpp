#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;


//Set up physx global variables
//Should be checked over to see what actually needs to be global and what doesnt
//TODO move to physics systems

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;


PxCooking* gCooking = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

/* TODO: put this in a function:
if (physx_actors.size() == 0) {
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), 0 });
	}
	else {
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), physx_actors.back().actorId + 1 });
	}
*/
std::vector<physx_actor_entity> physx_actors;
std::vector<PowerUp> active_items;


snippetvehicle::VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxRigidStatic* garageDoor = NULL;
bool garageDoorOpen = false;
bool garageDoorPrev = false;

PxActor* activeLevelActorPtr = NULL;