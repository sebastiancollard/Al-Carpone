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
	//BoxTrigger* headlights;
	float baseDetectionRadius = 30.f;
	float detectionRadius = baseDetectionRadius;
	float donutRadius = baseDetectionRadius * 1.5f;

	float baseJailRadius = 20.f;
	float jailRadius = baseJailRadius;

	glm::vec3 playerPos;


	DrivingNodes* dNodes;
	bool isStunned;

	float maxChaseTime = 10.f;

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
	void handleStuckPatrol();

	void update(Player& player, State& state);

	float distanceToPlayer = 999;

	bool playerInTrigger; //Player in trigger volume
	bool playerInSight; //Raycast to player successful
	bool playerDetected; //playerInTrigger && playerInSight
	bool playerInJailRadius; 
	bool playerArrestable; //playerInJailRadius && playerInSight

	bool findingPatrolRoute = true;
	bool chaseJustEnded = false;
	
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
	void eatDonut(glm::vec3 donutPos);
};