#pragma once

#include <iostream>
#include <queue>
#include "Vehicle.h"
#include "State.h"


// For compiler not to complain
class State;


/*
Player Entity Class
 - Used for containing Car Model, Transformation, and car inputs
*/




class Player : public Vehicle {

private:

	bool footIsOnGas;
	bool footIsOnBrake;

	double cash = 0;				//Amount of cash the player has on-hand. Private variable with accessors & mutators?


public:

	bool isSeen = false;

	Player() {}

	//Call parrent constructor
	Player(int ID) : Vehicle(VEHICLE_TYPE::AL_CARPONE, ID, PxVec3(0, 0, 0)) {}


	// Must be called after graphics system is initalized!
	void createModel();


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window, State& state);

	bool footOnGas();
	bool footOnBrake();

	// Robbing Mehcanism
	int getCash();
	void addCash(double amount);
	void setCash(double amount);
	void setPos(PxTransform T);
};
