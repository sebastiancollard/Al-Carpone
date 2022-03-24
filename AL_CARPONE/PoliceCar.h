#pragma once

#include "PxPhysicsAPI.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Vehicle.h"
#include "Model.h"
#include "BoxTrigger.h"
#include "Player.h"
#include "State.h"

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
	bool isStunned;


	PoliceCar() {}

	// Call parent constructor
	PoliceCar(int ID);

	// Must be called after graphics system is initalized!
	void createModel();

	void handle(GLFWwindow* window, Player& player, State& state);
	void stun(double seconds = 5);
	void startChase();
	void hardReset();

private:

	double idleTime = 0;
	double chaseTime = 0;

	void idle(double timestep);
	void patrol(GLFWwindow* window);
	void chase(GLFWwindow* window, Player& player, double timestep);

	void driveTo(glm::vec3 targetPos);
};