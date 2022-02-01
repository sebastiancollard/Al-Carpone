#pragma once
class Player : public Vehicle{
public:
	
	Player() {}

	//Call parrent constructor
	Player(unsigned int ID) : Vehicle(ID) {}


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed
			if (vehicleInAir) {
				glm::vec3 left = -getRight();
				vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(left.x, left.y, left.z));
			}
		}
			
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
			if (vehicleInAir) {
				glm::vec3 right = getRight();
				vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(right.x, right.y, right.z));
			}
		}
																		// Set as an else if for now seeing as you normally can't accelerate frontwards/backwards at the same time...
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		// Add left turn to the input queue if 'A' is pressed
			if (vehicleInAir) {
				glm::vec3 back = -getDir();
				vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(back.x, back.y, back.z));
			}
		}
			
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	// Add right turn to the input queue if 'D' is pressed
			if (vehicleInAir) {
				glm::vec3 front = getDir();
				vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(front.x, front.y, front.z));
			}
		}
			
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);			// Add handbrake to the input queue if 'spacebar' is pressed
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !state.shift_isHeld) {
			glm::vec3 front = getDir();
			vehiclePtr->getRigidDynamicActor()->addForce(10000.0f * PxVec3(front.x, front.y, front.z), physx::PxForceMode::Enum::eIMPULSE);
			state.shift_isHeld = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
			state.shift_isHeld = false;
		}
		
	}
};
