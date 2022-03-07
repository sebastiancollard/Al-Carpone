#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Vehicle.h"
#include "Building.h"
#include "PoliceCar.h"


//Locked position and orientation. Used for third person view behind the car.
#define CAMERA_MODE_BOUND 0
//Player has full control of position and orientation of the camera, loses control of the car.
//Used for debugging.
#define CAMERA_MODE_UNBOUND_FREELOOK 2


// For compiler not to complain
class Building;
class Vehicle;
class PoliceCar;


enum LEVELS {
	LEVEL_MAIN = 0,
	LEVEL_TUNING,
	LEVEL_RACETRACK
};

enum GAMESTATE {
	GAMESTATE_MAIN_MENU,
	GAMESTATE_PAUSE_MENU,
	GAMESTATE_INGAME,
	GAMESTATE_CORNERSTORE
};




//Stores information pertaning to the current state of the game.
class State {
public:
	
	GAMESTATE gamestate = GAMESTATE::GAMESTATE_MAIN_MENU;

	// Time
	double currTime = 0.;				
	double prevTime = 0.;				
	double timeStep = 1.0f / 60.0f;		
	float simulationSpeed = 2.f;
	double timeSinceLastFpsUpdate = 0;

	// Modes
	unsigned int cameraMode = CAMERA_MODE_BOUND;
	bool debugMode = false;
	unsigned int selectedLevel = 0;

	//If this is flipped to true, the program should exit.
	bool terminateProgram = false;
	bool gameWon = false;

	std::vector<Vehicle*> activeVehicles = {};
	std::vector<PoliceCar*> activePoliceVehicles = {};
	std::vector<Building*> buildings = {};		// Bank = [0], OtherBuildings = [1:]


	//Used to check for single input to prevent the key from being spammed.
	//This could be moved to some sort of general input class at somepoint.
	bool Q_isHeld = false;
	bool R_isHeld = false;
	bool S_isHeld = false;
	bool W_isHeld = false;
	bool M_isHeld = false;
	bool N_isHeld = false;
	bool f5_isHeld = false;
	bool f_isHeld = false;
	bool enter_isHeld = false;
	bool shift_isHeld = false;
	bool space_isHeld = false;
	bool down_isHeld = false;
	bool up_isHeld = false;
	bool escape_isHeld = false;
	//CONTROLLER input
	bool dpad_downisHold = false;
	bool dpad_upisHold = false;
	//Updates the state's time-sensitive variables.
	void updateTime() {
		currTime = glfwGetTime();
		timeStep = currTime - prevTime;
		prevTime = currTime;
		timeSinceLastFpsUpdate += timeStep;
	}

	//Toggles the camera mode between bound and unbound.
	void toggleCameraMode() {
		if (cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
			cameraMode = CAMERA_MODE_BOUND;
		}
		else {
			cameraMode = CAMERA_MODE_UNBOUND_FREELOOK;
		}
	}

};
