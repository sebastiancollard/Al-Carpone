#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;


//Set up physx global variables
//Should be checked over to see what actually needs to be global and what doesnt

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;


PxCooking* gCooking = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

std::vector<physx_actor_entity> physx_actors;

snippetvehicle::VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxActor* activeLevelActorPtr = NULL;