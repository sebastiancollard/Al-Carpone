#pragma once

// Graphics Libraries
#include <glm/gtc/type_ptr.hpp>
#include "constants.h"

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
#include "miscFunctions.h"

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

#include "PowerUp.h"

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

