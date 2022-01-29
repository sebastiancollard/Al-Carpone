#pragma once

//Locked position and orientation. Used for third person view behind the car.
#define CAMERA_MODE_BOUND 0
//Player has full control of position and orientation of the camera, loses control of the car.
//Used for debugging.
#define CAMERA_MODE_UNBOUND_FREELOOK 2

//Stores information pertaning to the current state of the game.
class State {
public:

	//Current time in game
	double currTime = 0.;
	//Previous time (time at last currTime update)
	double prevTime = 0.;
	//The ammount of time since last time update
	double timeStep;

	//Number of frames since last fps update
	unsigned int frameCount = 0;
	unsigned int cameraMode = CAMERA_MODE_BOUND;
	

	//If this is flipped to true, the program should exit.
	bool terminateProgram = false;

	//Used for q input to prevent the key from being spammed.
	//This could be moved to some sort of general input class at somepoint.
	bool Q_isHeld = false;

	//Updates the state's time-sensitive variables.
	void updateTime() {
		currTime = glfwGetTime();
		timeStep = currTime - prevTime;
		++frameCount;
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
