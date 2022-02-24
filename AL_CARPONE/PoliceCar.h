#pragma once

#include "PxPhysicsAPI.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Vehicle.h"
#include "Model.h"
#include "BoxTrigger.h"

/*
PoliceCar Entity Class
 - Used for containing Car Model, Transformation, and AI
*/

enum class AISTATE {
	IDLE,
	PATROL,
	CHASE
};

class PoliceCar : public Vehicle {

public:

	AISTATE state = AISTATE::PATROL;
	BoxTrigger* headlights = NULL; //TODO not initialized

	PoliceCar() {}

	// Call parent constructor
	PoliceCar(int ID) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(10.0f, 0, 0)) {}

	void handle(GLFWwindow* window, glm::vec3 playerPos);

	// Must be called after graphics system is initalized!
	void createModel();

private:

	float idleTime = 0;

	void idle();
	void patrol(GLFWwindow* window);
	void chase(GLFWwindow* window, glm::vec3 playerPos);

};