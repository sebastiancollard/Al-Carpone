#pragma once

#include "PxPhysicsAPI.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Vehicle.h"
#include "Model.h"
#include "BoxTrigger.h"
#include "Player.h"
#include "State.h"
#include "DrivingNodes.h"
#include <thread>

/*
PoliceCar Entity Class
 - Used for containing Car Model, Transformation, and AI
*/

class Player;
class State;


enum class AISTATE {
	IDLE,
	PATROL,
	CHASE
};

class PoliceCar : public Vehicle {

public:

	AISTATE ai_state = AISTATE::PATROL;
	BoxTrigger* headlights;
	DrivingNodes* dNodes;


	PoliceCar() {}

	// Call parent constructor
	PoliceCar(int ID, DrivingNodes* drivingNodes);

	// Must be called after graphics system is initalized!
	void createModel();
	
	void handle(glm::vec3 playerPos,double timestep);
	void stun(double seconds = 5);
	void reset();
	void startChase();
	void hardReset();

	void update(glm::vec3 playerPos,double timeStep);

	bool playerInTrigger; //Player in trigger volume
	bool playerInSight; //Raycast to player successful
	bool playerDetected; //playerInTrigger && playerInSight
	
	//debug
	glm::vec3 getTargetDirection() {
		return glm::normalize(targetPosition - getPos());
	}
	glm::vec3 getTargetNodeLocation() {
		return targetPosition;
	}

	
	glm::vec3 myPos;
	glm::vec3 myDir;
	float myForwardSpeed;
	bool shouldReset;

	double chaseTime = 0;

private:


	double prevTime;

	double idleTime = 0;
	double stuckTime = 0;
	double reverseTime = 0;
	double brakeTime = 0;
	double airTime = 0;

	bool targetingPlayer = false;
	

	std::thread myThread;

	void idle(double timestep);
	void patrol();
	void chase(glm::vec3 playerPos,double timestep);
	void reverse(double timestep, glm::vec3 playerPos);
	void brake(double timestep);

	void driveTo(glm::vec3 targetPos);
};