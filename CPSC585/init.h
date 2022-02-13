#pragma once

#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL; }
#define M_PI 3.14159265358979323846





#define BANK_BUILDING_PATH "models/al_carpone/bank_box.obj"


#define NEAR_CLIPPING_PLANE 0.01f
#define FAR_CLIPPING_PLANE 1000.f

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<iostream>

#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

#include "GraphicsSystem.h"

#include "Model.h"
#include "shader.h"
#include "Skybox.h"


#include "PxPhysicsAPI.h"

#include "vehicle/PxVehicleUtil.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"

#include "snippetcommon/SnippetPrint.h"
#include "snippetcommon/SnippetPVD.h"


std::vector<std::string> level_texture_paths{
"models/tuning_testlevel/tuning_testlevel.obj",
"models/racetrack/racetrack.obj",
"models/ai_testlevel/ai_testlevel.obj",
"models/city_prototype/city_prototype.obj" 
};





using namespace physx;
using namespace snippetvehicle;

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
void printVec3(std::string name, PxVec3 v) {
	printf("%s: <%.2f, %.2f, %.2f>\n", name, v.x, v.y, v.z);
}

//Vehicle tuning settings
#include"physXVehicleSettings.h"


// Physx Globals
#include "physx_globals.h"


#include"State.h"
State state;

// Window elements
#include "DebugPanel.h"
#include "MainMenu.h"
#include "UI.h"

#include"Vehicle.h"
#include"Player.h"
#include "PoliceCar.h"
Player player;

#include "Bank.h"
Bank bank;

std::vector<Vehicle*> activeVehicles;

#include "Camera.h"
//All of the physx functions needed for the vehicle sdk
#include"physXVehicleFunctions.h"
//All of the physx functions needed for setup + running
#include"physXGeneralFunctions.h"
//Miscelanious functions who dont have a home yet
#include"miscFunctions.h"




