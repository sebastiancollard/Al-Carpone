#pragma once

#include <iostream>
#include <queue>

#define CAR_CHASSIS_PATH "models/al_carpone/chassis_carpone.obj"
#define CAR_LWHEEL_PATH "models/al_carpone/car_Lwheel.obj"
#define CAR_RWHEEL_PATH "models/al_carpone/car_Rwheel.obj"



class Player : public Vehicle {
private:
	bool can_rob = false;				//If player "collides" with trigger capsule, this shoudl be set to true
	int cash = 0;						//Amount of cash the player has on-hand. Private variable with accessors & mutators?


public:
	
	Player() {}

	//Call parrent constructor
	Player(VEHICLE_TYPE type) : Vehicle(type, 0, PxVec3(0,0,0)) {}


	// Must be called after graphics system is initalized!
	void createModel() {

		Model car_chassis(CAR_CHASSIS_PATH);
		Model car_lwheel(CAR_LWHEEL_PATH);
		Model car_rwheel(CAR_RWHEEL_PATH);

		car = new CarModel4W(car_chassis, car_lwheel, car_rwheel);
	}


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
		/*
		this part handles controller input,
		line "std::cout << controller_name << std::endl;" will print out the current input controller's name
			"XInput Gamepad (GLFW)" for xbox (2.5Hz) && usb connection
			"PS4 controller" for ps4 (usb connection)
			"Nintendo Switch Pro Controller" for bluetooth connection
		Since NS pro trigger button is not linear it deos not have axes, the value it will be either -1 or 1
		*/
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
		{
			bool accel = false;
			//get controller name
			//const char* controller_name = glfwGetGamepadName(GLFW_JOYSTICK_1);
			//std::cout << controller_name << std::endl;

			GLFWgamepadstate state;
			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
			{
				float forwardOrbackward, leftOrRightturn;
				forwardOrbackward = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				leftOrRightturn = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];

				if (forwardOrbackward >= 0.1)
				{
					inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);
					if (vehicleInAir) {
						glm::vec3 right = getRight();
						vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(right.x, right.y, right.z));
					}
					accel = true;
				}
				if (forwardOrbackward <= -0.1)
				{
					inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);
					if (vehicleInAir) {
						glm::vec3 left = -getRight();
						vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(left.x, left.y, left.z));
					}
					accel = true;
				}
				if (leftOrRightturn >= 0.5)
				{
					inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
					if (vehicleInAir) {
						glm::vec3 front = getDir();
						vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(front.x, front.y, front.z));
					}
				}
				if (leftOrRightturn <= -0.5)
				{
					inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
					if (vehicleInAir) {
						glm::vec3 back = -getDir();
						vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(back.x, back.y, back.z));
					}
				}
				if (leftOrRightturn > -0.5 && leftOrRightturn < 0.5 && forwardOrbackward < 0.1 && forwardOrbackward > -0.1 && accel == true)
				{
					inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);
					accel = false;
				}

			}
		}

	}


	int getCash() { 
		return cash; 
	}
	bool canRob() { 
		return can_rob && state.selectedLevel == 0; 
	}
	
	void setRob(bool b) { 
		can_rob = b; 
	}
	void addCash(int amount) {
		cash += amount;
	}
	void setCash(int amount) {
		cash = amount;
	}
};
