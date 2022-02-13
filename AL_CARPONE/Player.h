#pragma once

#include <iostream>
#include <queue>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "Vehicle.h"
#include "State.h"



/*
Player Entity Class
 - Used for containing Car Model, Transformation, and car inputs
*/


class Player : public Vehicle {

private:

	bool can_rob = false;		//If player "collides" with trigger capsule, this shoudl be set to true
	int cash = 0;				//Amount of cash the player has on-hand. Private variable with accessors & mutators?


public:

	Player() {}

	//Call parrent constructor
	Player(int ID) : Vehicle(VEHICLE_TYPE::AL_CARPONE, ID, PxVec3(0,0,0)) {}


	// Must be called after graphics system is initalized!
	void createModel();


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window, State& state);


	// Robbing Mehcanism
	int getCash();
	bool canRob(State& state);
	void setRob(bool b);
	void addCash(int amount);
	void setCash(int amount);
};
