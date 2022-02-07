#pragma once

#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL; }
#define M_PI 3.14159265358979323846


#define CAR_CHASSIS_PATH "models/al_carpone/chassis_carpone.obj"
#define CAR_LWHEEL_PATH "models/al_carpone/car_Lwheel.obj"
#define CAR_RWHEEL_PATH "models/al_carpone/car_Rwheel.obj"

#define ACTIVE_LEVEL_TEXTURED_MODEL_PATH "models/testlevel/tuning_testlevel.obj"
#define ACTIVE_LEVEL_PHYSX_MODEL_PATH "models/testlevel/tuning_testlevel_physx_model.obj"

//#define ACTIVE_LEVEL_TEXTURED_MODEL_PATH "models/testlevel/ai_testlevel.obj"
//#define ACTIVE_LEVEL_PHYSX_MODEL_PATH "models/testlevel/ai_testlevel_physx_model.obj"


#define NEAR_CLIPPING_PLANE 0.01f
#define FAR_CLIPPING_PLANE 1000.f

//Screen width and height. May want to change this to a dynamic value eventually.
const unsigned int SCREEN_WIDTH = 1920 * 0.75f;
const unsigned int SCREEN_HEIGHT = 1080 * 0.75f;

#include<glad/glad.h>

#include<iostream>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

//Used for debugging
void printMat4(glm::mat4 m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%.2f ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void printVec3(std::string name, glm::vec3 v) {
	printf("%s: <%.2f, %.2f, %.2f>\n", name, v.x, v.y, v.z);
}

#include"shader.h"
#include "Model.h"



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

struct physx_actor_entity
{
	PxRigidActor* actorPtr;
	PxU32 actorId;
};
std::vector<physx_actor_entity> physx_actors;

VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;


#include"State.h"
State state;

#include "DebugPanel.h"
#include"Vehicle.h"
#include"Player.h"

Player player;
std::vector<Vehicle*> activeVehicles;

#include "Camera.h"
//All of the physx functions needed for the vehicle sdk
#include"physXVehicleFunctions.h"
//All of the physx functions needed for setup + running
#include"physXGeneralFunctions.h"
//Miscelanious functions who dont have a home yet
#include"miscFunctions.h"



