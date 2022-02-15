#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Vehicle.h"


//Locked position and orientation. Used for third person view behind the car.
#define CAMERA_MODE_BOUND 0
//Player has full control of position and orientation of the camera, loses control of the car.
//Used for debugging.
#define CAMERA_MODE_UNBOUND_FREELOOK 2

//Stores information pertaning to the current state of the game.
class State {
public:
	
	double currTime = 0.;				//Current time in game
	double prevTime = 0.;				//Previous time (time at last currTime update)
	double timeStep = 1.0f / 60.0f;		//The ammount of time since last time update
	float simulationSpeed = 2.f;
	double timeSinceLastFpsUpdate = 0;

	// Modes
	unsigned int cameraMode = CAMERA_MODE_BOUND;
	bool debugMode = false;
	bool mainMenu = true;
	unsigned int selectedLevel = 0;

	//If this is flipped to true, the program should exit.
	bool terminateProgram = false;

	std::vector<Vehicle*> activeVehicles = {};

	//Used to check for single input to prevent the key from being spammed.
	//This could be moved to some sort of general input class at somepoint.
	bool Q_isHeld = false;
	bool R_isHeld = false;
	bool S_isHeld = false;
	bool W_isHeld = false;
	bool f5_isHeld = false;
	bool shift_isHeld = false;
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
