#include "Player.h"

#define CAR_CHASSIS_PATH "models/al_carpone/chassis_carpone.obj"
#define CAR_LWHEEL_PATH "models/al_carpone/car_Lwheel.obj"
#define CAR_RWHEEL_PATH "models/al_carpone/car_Rwheel.obj"


void Player::createModel() {

	Model car_chassis(CAR_CHASSIS_PATH);
	Model car_lwheel(CAR_LWHEEL_PATH);
	Model car_rwheel(CAR_RWHEEL_PATH);

	car = new CarModel4W(car_chassis, car_lwheel, car_rwheel);
}


///////////////////////////////////////////////////////////////////////
// ROBING FUNCTIONS
///////////////////////////////////////////////////////////////////////



int Player::getCash() {
	return cash;
}
bool Player::canRob(State& state) {
	return can_rob && state.selectedLevel == 0;
}

void Player::setRob(bool b) {
	can_rob = b;
}
void Player::addCash(int amount) {
	cash += amount;
}
void Player::setCash(int amount) {
	cash = amount;
}

///////////////////////////////////////////////////////////////////////
// POWER-UP FUNCTIONS
///////////////////////////////////////////////////////////////////////
void Player::setDetectable(bool b) {
	detectable = b;
}
bool Player::isDetectable() {
	return detectable;
}

PowerUp* Player::getPower() {
	return &equippedPower;
}

void Player::usePower() {
	if (equippedPower.getType() == CAMOUFLAGE) {
		equippedPower.activateTimed();
		detectable = false;			//nowhere is this reset to true atm, must check to see if player is detectable AND if their ability is active
	}
	else {
		equippedPower.dropOrThrow();
	}
}

bool Player::canChooseTool(State& state) {
	return can_choosePowerTool && state.selectedLevel == 0;
}

void Player::setChooseTool(bool b) {
	can_choosePowerTool = b;
}

bool Player::footOnGas() {
	return footIsOnGas;
}

bool Player::footOnBrake() {
	return footIsOnBrake;
}

///////////////////////////////////////////////////////////////////////
// INPUT HANDLING
///////////////////////////////////////////////////////////////////////


// Handle all key inputs relevant to driving
void Player::handleInput(GLFWwindow* window, State& state)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed
		if (vehicleInAir) {
			glm::vec3 left = -getRight();
			vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(left.x, left.y, left.z));
		}

		state.W_isHeld = true;
	}
	else{
		state.W_isHeld = false;
	}

	footIsOnGas = state.W_isHeld;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
		if (vehicleInAir) {
			glm::vec3 right = getRight();
			vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(right.x, right.y, right.z));
		}
	}
	// Set as an else if for now seeing as you normally can't accelerate frontwards/backwards at the same time...
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		// Add left turn to the input queue if 'A' is pressed
		if (vehicleInAir) {
			glm::vec3 back = -getDir();
			vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(back.x, back.y, back.z));
		}
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	// Add right turn to the input queue if 'D' is pressed
		if (vehicleInAir) {
			glm::vec3 front = getDir();
			vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(front.x, front.y, front.z));
		}
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);			// Add handbrake to the input queue if 'spacebar' is pressed
		state.space_isHeld = true;
	}
	else state.space_isHeld = false;

	footIsOnBrake = state.space_isHeld;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !state.shift_isHeld) {
		glm::vec3 front = getDir();
		vehiclePtr->getRigidDynamicActor()->addForce(10000.0f * PxVec3(front.x, front.y, front.z), physx::PxForceMode::Enum::eIMPULSE);
		state.shift_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		state.shift_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		usePower();
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
			//forwardOrbackward = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
			leftOrRightturn = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];

			if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > -1)
			{
				inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
				if (vehicleInAir) {
					glm::vec3 right = getRight();
					vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(right.x, right.y, right.z));
				}
				//std::cout << "left trigger: " << state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] << std::endl;	//press = 1, idle = -1
			}
			if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > -1)
			{
				inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed
				if (vehicleInAir) {
					glm::vec3 left = -getRight();
					vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(left.x, left.y, left.z));
				}
				//std::cout << "right trigger: " << state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] << std::endl;
			}
			if (leftOrRightturn < -0.15)
			{
				inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		
				if (vehicleInAir) {
					glm::vec3 back = -getDir();
					vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(back.x, back.y, back.z));
				}
			}
			else if (leftOrRightturn > 0.15)
			{
				inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	
				if (vehicleInAir) {
					glm::vec3 front = getDir();
					vehiclePtr->getRigidDynamicActor()->addTorque(500.0f * PxVec3(front.x, front.y, front.z));
				}
			}

		}
	}


}
