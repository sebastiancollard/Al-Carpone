#pragma once

#include <iostream>
#include <queue>
#include "Vehicle.h"
#include "State.h"
#include "PowerUp.h"


/*
Player Entity Class
 - Used for containing Car Model, Transformation, and car inputs
*/


class Player : public Vehicle {

private:
	
	static Player player;		//static instance of itself

	bool footIsOnGas;
	bool footIsOnBrake;

	bool detectable = true;		//does not do anything yet. flag that specifies whether the player is detectable to enemies
	bool can_rob = false;		//If player "collides" with trigger capsule, this shoudl be set to true
	double cash = 0;				//Amount of cash the player has on-hand. Private variable with accessors & mutators?

	double cashRateMultiplier = 5;

	PowerUp equippedPower =  PowerUp();
	bool can_choosePowerTool = false;

public:

	Player() {}

	//Call parrent constructor
	Player(int ID) : Vehicle(VEHICLE_TYPE::AL_CARPONE, ID, PxVec3(0, 0, 0)) {}


	// Must be called after graphics system is initalized!
	void createModel();


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window, State& state);

	bool footOnGas();
	bool footOnBrake();

	bool canExit(State& state);

	// Robbing Mehcanism
	int getCash();
	bool canRob(State& state);
	void setRob(bool b);
	void addCash(int amount);
	void setCash(int amount);
	void setPos(PxTransform T);
	
	//Power-up related mechanisms
	bool isDetectable();
	void setDetectable(bool can_detect);
	PowerUp* getPower();
	void usePower();
	void updatePower();
	bool canChooseTool(State& state);
	void setChooseTool(bool b);
};
