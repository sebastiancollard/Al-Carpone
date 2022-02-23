#pragma once

#include "PxPhysicsAPI.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Vehicle.h"
#include "Model.h"

/*
PoliceCar Entity Class
 - Used for containing Car Model, Transformation, and AI
*/

class PoliceCar : public Vehicle {

public:

	PoliceCar() {}

	// Call parent constructor
	PoliceCar(int ID) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(10.0f, 0, 0)) {}

	void handle(GLFWwindow* window, glm::vec3 playerPos);

	// Must be called after graphics system is initalized!
	void createModel();
};