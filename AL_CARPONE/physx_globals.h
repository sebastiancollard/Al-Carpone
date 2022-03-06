#pragma once
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "PxPhysicsAPI.h"
#include <vector>
#include "Model.h"

using namespace physx;


//Set up physx global variables
//Should be checked over to see what actually needs to be global and what doesnt

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

struct simple_renderable_object {
	PxRigidActor* actorPtr;
	Model model;
};
extern std::vector<simple_renderable_object> simple_renderables;

extern snippetvehicle::VehicleSceneQueryData* gVehicleSceneQueryData;
extern PxBatchQuery* gBatchQuery;

extern PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs;

extern PxRigidStatic* gGroundPlane;
extern PxActor* activeLevelActorPtr;