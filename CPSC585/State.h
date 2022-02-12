#pragma once
#include "Player.h"


/*
GameState containing most of the important variables
*/


//MENU

//Locked position and orientation. Used for third person view behind the car.
#define CAMERA_MODE_BOUND 0
//Player has full control of position and orientation of the camera, loses control of the car.
//Used for debugging.
#define CAMERA_MODE_UNBOUND_FREELOOK 2

//Stores information pertaning to the current state of the game.
class State {
public:

	// Game Objects
	Player* player;


	// TIME
	double currTime = 0.;			//Current time in game
	double prevTime = 0.;			//Previous time (time at last currTime update)
	double timeStep = 1.0f / 60.0f;	//The ammount of time since last time update
	float simulationSpeed = 2.f;
	double timeSinceLastFpsUpdate = 0;

	// CAMERA
	unsigned int cameraMode = CAMERA_MODE_BOUND;

	//If this is flipped to true, the program should exit.
	bool terminateProgram = false;

	// Menus and Windows
	bool debugMode = false;			//Debug Window
	bool mainMenu = true;
	unsigned int selectedMainMenuOption = 0;

	unsigned int selectedLevel = 0;

	//Used to check for single input to prevent the key from being spammed.
	//TODO: This could be moved to some sort of general input class at somepoint.
	bool Q_isHeld = false;
	bool R_isHeld = false;
	bool S_isHeld = false;
	bool W_isHeld = false;
	bool f5_isHeld = false;
	bool shift_isHeld = false;
	bool down_isHeld = false;
	bool up_isHeld = false;
	bool escape_isHeld = false;
	

	
	State();

	void updateTime();			//Updates the state's time-sensitive variables.
	void toggleCameraMode();	//Toggles the camera mode between bound and unbound.

};
