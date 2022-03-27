#pragma once

static enum DEBUGMODE {
	FALSE,
	NOJAIL,
	NOALERT,
	NOCOPS
};

DEBUGMODE debugmode = DEBUGMODE::FALSE;

// Graphics Libraries
#include <glm/gtc/type_ptr.hpp>
#include "constants.h"
#include <ft2build.h>

// Physics Libraries
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "physx_globals.h"

// General Libraries
#include <iostream>
#include <vector>

// Systems
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "AudioSystem.h"
#include "specialInputHandler.h"

// Window elements
#include "DebugPanel.h"
#include "MainMenu.h"
#include "PauseMenu.h"
#include "UI.h"

// Game Elements
#include "Skybox.h"
#include "Camera.h"
#include "State.h"

// Entities
#include "Player.h"
#include "PoliceCar.h"
#include "Bank.h"
#include "Exit.h"
#include "CornerStore.h"
#include "Garage.h"
#include "PowerUp.h"
#include "SelectItem.h"
#include "AI.h"


struct DebugTools {
	Model red_arrow, blue_arrow, grey_arrow, red_node, blue_node, grey_node;
	DebugTools() :
		red_arrow("models/debug/red_arrow.obj"), blue_arrow("models/debug/blue_arrow.obj"), grey_arrow("models/debug/grey_arrow.obj"),
		red_node("models/debug/red_node.obj"), blue_node("models/debug/blue_node.obj"), grey_node("models/debug/grey_node.obj") {}

};

//freetype
#include "textRenderer.h"


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

