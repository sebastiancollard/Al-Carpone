
#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL; }

const unsigned int width = 800;
const unsigned int height = 800;

#include"Mesh.h"
#include<iostream>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

#include"Texture.h"
#include"shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"freeCamera.h"

#include "PxPhysicsAPI.h"

#include "vehicle/PxVehicleUtil.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"

#include "snippetcommon/SnippetPrint.h"
#include "snippetcommon/SnippetPVD.h"

using namespace physx;
using namespace snippetvehicle;

//All of the vehicle tuning settings
#include"physXVehicleSettings.h"

#include"Player.h"
Player player;

#include"State.h"
State state;

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

VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxVehicleDrive4W* gVehicle4W = NULL;

bool gIsVehicleInAir = true;

struct physx_actor_entity
{
	PxRigidActor* actorPtr;
	PxU32 actorId;
};
std::vector<physx_actor_entity> physx_actors;

//All of the physx functions needed for the vehicle sdk
#include"physXVehicleFunctions.h"
//All of the physx functions needed for setup + running
#include"physXGeneralFunctions.h"
//Miscelanious functions who dont have a home yet
#include"miscFunctions.h"