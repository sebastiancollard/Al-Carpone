#pragma once

#include <iostream>
#include <queue>
#include "Vehicle.h"
#include "State.h"
#include "PowerUp.h"

// For compiler not to complain
class State;


/*
Player Entity Class
 - Used for containing Car Model, Transformation, and car inputs
*/


class Player : public Vehicle {

private:
	
	static Player player;		//static instance of itself

	bool footIsOnGas = false;
	bool footIsOnBrake = false;

	CarModel4W* player_model;
	CarModel4W* police_model;
	

	bool detectable = true;					//Flag that specifies whether the player is detectable to enemies
	VEHICLE_TYPE model_type = AL_CARPONE;	//Used for model switching when using the camouflage power
	unsigned int cash = 0;					//Amount of cash the player has on-hand. Private variable with accessors & mutators?

	double cashRateMultiplier = 1;

	PowerUp equippedPower =  PowerUp();
	bool can_choosePowerTool = false;


	//robbing
	float timeSpentRobbing = 0;
	



public:
	float basecashRobbedPerFrame = 1000;
	float cashRobbedPerFrame = basecashRobbedPerFrame;

	float initAlarmChancePerCheck = 0.1f;
	float baseAlarmChancePerCheck = initAlarmChancePerCheck;
	float alarmChancePerCheck = baseAlarmChancePerCheck;

	float alarmCheckTimer = 0.f;

	float initAlarmCheckInterval = 2.f;
	float baseAlarmCheckInterval = initAlarmCheckInterval;
	float alarmCheckInterval = baseAlarmCheckInterval;


	float jailTimer = 0;
	float flippedOverTimer = 0.f;

	bool isSeen = false;

	bool toggleHeadlights = true;

	bool canFlip = false;
	float drawRadius = 0.f;
	unsigned int minimapMode = 0;
	unsigned int numUpgradesPurchased = 0;

	float headlightForwardOffset1 = 2.174;
	float headlightHorizontalOffset1 = 0.806;
	float headlightVerticalOffset1 = -0.101;

	float headlightForwardOffset2 = 2.174;
	float headlightHorizontalOffset2 = 0.809;
	float headlightVerticalOffset2 = 0.089;


	Player() {}

	//Call parrent constructor
	Player(int ID) : Vehicle(VEHICLE_TYPE::AL_CARPONE, ID, PxVec3(0, 0, 0)) {}


	// Must be called after graphics system is initalized!
	void createModel();
	void setCurrentModel(VEHICLE_TYPE type);


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window, State& state);

	bool footOnGas();
	bool footOnBrake();

	bool canExit(State& state);

	void sendToJail(State& state);
	void rob(State& state);

	bool beingChased(State& state);

	bool isFlippedOver();

	std::pair<std::pair<glm::vec3, glm::vec3>, std::pair<glm::vec3, glm::vec3>> getHeadlightPositions();

	// Robbing Mehcanism
	int getCash();
	void addCash(double amount);
	void setCash(double amount);
	void setPos(PxTransform T);
	
	//Power-up related mechanisms
	bool isDetectable();
	void setDetectable(bool can_detect);
	VEHICLE_TYPE getCurrentModelType();
	PowerUp* getPower();
	void usePower();
	bool canChooseTool(State& state);
	void setChooseTool(bool b);
};
