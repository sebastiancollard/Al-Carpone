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
	
	void handle(GLFWwindow* window, Player& player, State& state);
	void stun(double seconds = 5);
	void reset();
	void startChase();
	void hardReset();

	
	
	//debug
	glm::vec3 getTargetDirection() {
		return glm::normalize(targetPosition - getPos());
	}
	glm::vec3 getTargetNodeLocation() {
		return targetPosition;
	}

private:

	double idleTime = 0;
	double chaseTime = 0;
	double stuckTime = 0;
	double reverseTime = 0;

	bool targetingPlayer = false;
	

	void idle(double timestep);
	void patrol(GLFWwindow* window);
	void chase(GLFWwindow* window, Player& player, double timestep);
	void reverse(double timestep);

	void driveTo(glm::vec3 targetPos);
};