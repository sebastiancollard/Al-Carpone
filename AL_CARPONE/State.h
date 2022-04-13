#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Player.h"
#include "Vehicle.h"
#include "Building.h"
#include "PoliceCar.h"
#include "Player.h"



//Locked position and orientation. Used for third person view behind the car.
#define CAMERA_MODE_BOUND 0
//Player has full control of position and orientation of the camera, loses control of the car.
//Used for debugging.
#define CAMERA_MODE_UNBOUND_FREELOOK 2


// For compiler not to complain
class Player;
class Building;
class Vehicle;
class PoliceCar;
class AudioSystem;

enum GAMESTATE {
	GAMESTATE_MAIN_MENU,
	GAMESTATE_INGAME,
	GAMESTATE_CORNERSTORE,
	GAMESTATE_JAILED
};

enum BUILDINGS {
	BANK,
	GARAGE1,
	GARAGE2,
	GARAGE3,
	CORNERSTORE1,
	CORNERSTORE2,
	EXIT
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

	//If this is flipped to true, the program should exit.
	bool terminateProgram = false;
	bool gameWon = false;
	bool gameLost = false;

	

	//garage
	bool g_draw = false;

	//for sound effect
	int playlist_count = 0;

	std::vector<PoliceCar*> activePoliceVehicles = {};
	std::vector<PoliceCar*> inactivePoliceVehicles = {};
	Player* playerPtr;

	AudioSystem* audioSystemPtr;

	std::vector<Building*> buildings = 
	{
		NULL, //BANK
		NULL, //GARAGE1
		NULL, //GARAGE2
		NULL, //GARAGE3
		NULL, //CORNERSTORE1
		NULL, //CORNERSTORE2
		NULL  //EXIT
	};
				
	//Used to check for single input to prevent the key from being spammed.
	//This could be moved to some sort of general input class at somepoint.
	bool Q_isHeld = false;
	bool R_isHeld = false;
	bool S_isHeld = false;
	bool W_isHeld = false;
	bool M_isHeld = false;
	bool N_isHeld = false;
	bool H_isHeld = false;
	bool f5_isHeld = false;
	bool f_isHeld = false;
	bool enter_isHeld = false;
	bool shift_isHeld = false;
	bool space_isHeld = false;
	bool alt_isHeld = false;
	bool down_isHeld = false;
	bool up_isHeld = false;
	bool escape_isHeld = false;
	bool tab_isHeld = false;
	bool right_isHeld = false;
	bool left_isHeld = false;
	//CONTROLLER input
	bool dpad_downisHold = false;
	bool dpad_upisHold = false;
	bool dpad_leftIsHold = false;
	bool dpad_rightIsHold = false;
	bool dpad_startIsHeld = false;
	bool cross_isHeld = false;
	bool circle_isHeld = false;
	bool triangle_isHeld = false;
	bool square_isHeld = false;
	bool option_isHeld = false;
	bool right_thumb = false;

	void updateTime();
	void toggleCameraMode();
	void resetVehicles();
	void alertPolice();
	bool policeAlerted();
	float getAlertLevel();

};
