#include "Player.h"
#include "AudioSystem.h"

#define CAR_CHASSIS_PATH "models/al_carpone/chassis_carpone.obj"
#define CAR_LWHEEL_PATH "models/al_carpone/car_Lwheel.obj"
#define CAR_RWHEEL_PATH "models/al_carpone/car_Rwheel.obj"

#define POLICE_CAR_CHASSIS_PATH "models/police_car/police_car_chassis.obj"
#define POLICE_CAR_LWHEEL_PATH "models/police_car/police_car_wheel_left.obj"
#define POLICE_CAR_RWHEEL_PATH "models/police_car/police_car_wheel_right.obj"

static enum DEBUGMODE {
	FALSE,
	TRUE,
	NOJAIL,
	NOALERT,
	NOCOPS,
};

extern DEBUGMODE debugmode;


void Player::createModel(){
	Model car_chassis(CAR_CHASSIS_PATH);			//creates the player (al carpone) model
	Model car_lwheel(CAR_LWHEEL_PATH);
	Model car_rwheel(CAR_RWHEEL_PATH);
	player_model = new CarModel4W(car_chassis, car_lwheel, car_rwheel);
	car = player_model;

	Model police_chassis(POLICE_CAR_CHASSIS_PATH);	//creates a police car model to use with camouflage power
	Model police_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_rwheel(POLICE_CAR_RWHEEL_PATH);
	police_model = new CarModel4W(police_chassis, police_lwheel, police_rwheel);
}

void Player::setCurrentModel(VEHICLE_TYPE type) {
	if (type == AL_CARPONE) {
		car = player_model;
	}
	else {
		car = police_model;
	}
	model_type = type;
}

void Player::setPos(PxTransform T) {
	vehiclePtr->getRigidDynamicActor()->setGlobalPose(T);
}

void Player::sendToJail(State& state) {
	printf("GO TO JAIL!\n");
	reset();
	jailTimer = 0;
	state.gamestate = GAMESTATE::GAMESTATE_JAILED;
	return;
}

bool Player::beingChased(State& state) {
	for (PoliceCar* p : state.activePoliceVehicles)
		if (p->ai_state == AISTATE::CHASE) return true;
	return false;
}

std::pair<std::pair<glm::vec3, glm::vec3>, std::pair<glm::vec3, glm::vec3>> Player::getHeadlightPositions() {
	glm::vec3 origin = getPos();
	glm::vec3 uDir = getDir();
	glm::vec3 uRight = getRight();
	glm::vec3 uUp = getUp();

	float dForward = headlightForwardOffset1;
	float dRight = headlightHorizontalOffset1;
	float dUp = headlightVerticalOffset1;

	glm::vec3 frontCenter = origin + uUp * dUp + uDir * dForward;
	glm::vec3 rightDiff = uRight * dRight;

	std::pair<glm::vec3, glm::vec3> headlights_set1(glm::vec3(frontCenter - rightDiff), glm::vec3(frontCenter + rightDiff));

	dForward = headlightForwardOffset2;
	dRight = headlightHorizontalOffset2;
	dUp = headlightVerticalOffset2;

	frontCenter = origin + uUp * dUp + uDir * dForward;
	rightDiff = uRight * dRight;

	std::pair<glm::vec3, glm::vec3> headlights_set2(glm::vec3(frontCenter - rightDiff), glm::vec3(frontCenter + rightDiff));

	return std::pair<std::pair<glm::vec3, glm::vec3>, std::pair<glm::vec3, glm::vec3>>(headlights_set1, headlights_set2);
}

///////////////////////////////////////////////////////////////////////
// ROBING FUNCTIONS
///////////////////////////////////////////////////////////////////////

int Player::getCash() {
	return cash;
}
void Player::addCash(double amount) {
	cash += amount;
}
void Player::setCash(double amount) {
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

VEHICLE_TYPE Player::getCurrentModelType() {
	return model_type;
}

PowerUp* Player::getPower() {
	return &equippedPower;
}

void Player::gainPower(POWER_TYPE type, float price) {
	equippedPower.setType(type);
	equippedPower.setDuration(15.0f);
	setCash(getCash() - price);
	hasPower = true;
}

void Player::usePower() {
	//std:cout << equippedPower.getType() << std::endl;
	if (equippedPower.getType() == CAMOUFLAGE) {
		equippedPower.activateTimed();
		detectable = false;
	}
	else {
		equippedPower.dropOrThrow();
	}
	hasPower = false;
}

bool Player::canChooseTool(State& state) {
	return can_choosePowerTool;
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

bool Player::checkIsReversing() {
	return isReversing;
}

bool Player::canExit(State& state) {
	//Check that in exit triggerbox and that player cash > exit requirement
	return cash > 10.0f;
}


bool Player::isFlippedOver() {
	return glm::dot(getUp(), glm::vec3(0, 1, 0)) < 0.1f && glm::length(getLinearVelocity()) < 1.f;
}


void Player::rob(State& state) {
	
	inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);

	if (state.policeAlerted() || abs(getForwardVelocity()) > 0.1) return;
	
	timeSpentRobbing += state.timeStep;
	
	//printf("ALARMTIMER: %.2f\n", alarmCheckTimer);

	if (timeSpentRobbing > 1.0f) {
		alarmCheckTimer += state.timeStep;
		addCash(cashRobbedPerFrame * cashRateMultiplier * state.timeStep);
		state.audioSystemPtr->playRobLoop();
	}
	else {
		state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::OPEN_DUFFLE);
	}

	if (alarmCheckTimer > alarmCheckInterval) {
		//printf("ALARMTIMER: %.2f > %.2f\n", alarmCheckTimer, alarmCheckInterval);
		float random = (float)(rand() % 101) / 100.f;
		if (random < alarmChancePerCheck) {
			state.alertPolice();
			glm::vec3 pos = state.playerPtr->getPos();
			state.audioSystemPtr->triggerBankAlarm(pos.x, pos.y, pos.z);
			state.audioSystemPtr->stopSound(SOUND_SELECTION::ROB_LOOP);
			state.audioSystemPtr->stopSound(SOUND_SELECTION::OPEN_DUFFLE);
		}
		alarmCheckTimer = 0.f;
		alarmChancePerCheck *= 1.5f;
		std::cout << "alarm chance: " << alarmChancePerCheck * 100 << std::endl;
	}




// CASH_ROBBED_PER_FRAME* state.timeStep
}

void Player::tow() {
	reset();
	setCash(getCash() - 2500);
	if (getCash() < 0) setCash(0);
}

///////////////////////////////////////////////////////////////////////
// INPUT HANDLING
///////////////////////////////////////////////////////////////////////


// Handle all key inputs relevant to driving
void Player::handleInput(GLFWwindow* window, State& state)
{

	bool input_reverse = false;
	bool input_accelerate = false;
	bool input_handbrake = false;

	// Handle interactions
	if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)) {

		for (Building* b : state.buildings) {
			if (b == nullptr) continue;
			if (b->isInRange) {

				b->triggerFunction(*this, state);
				return;
			}
		}
		state.f_isHeld = true;
	}
	else  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		state.f_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		updateSpeed(1.0f);
		inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed
		if (vehicleInAir) {
			glm::vec3 right = getRight();
			vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(right.x, right.y, right.z));
		}
		input_accelerate = true;
		state.W_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
		state.W_isHeld = false;
	}


	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {

		if (!state.H_isHeld) {
			toggleHeadlights++;
			if (toggleHeadlights > 2) toggleHeadlights = 0;
			state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::GEAR_SWITCH0);
		}

		state.H_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
		state.H_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
		if (vehicleInAir) {
			glm::vec3 left = -getRight();
			vehiclePtr->getRigidDynamicActor()->addTorque(1500.0f * PxVec3(left.x, left.y, left.z));
		}
		state.S_isHeld = true;

		input_reverse = true;

	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		state.S_isHeld = false;
	}

	// Set as an else if for now seeing as you normally can't accelerate frontwards/backwards at the same time...
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		updateLeftSpeed(0.5f);
		inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		// Add left turn to the input queue if 'A' is pressed
		if (vehicleInAir) {
			glm::vec3 back = -getDir();
			vehiclePtr->getRigidDynamicActor()->addTorque((500.0f + (isFlippedOver() && canFlip ? 10000 : 0)) * PxVec3(back.x, back.y, back.z));
		}
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		updateRightSpeed(-0.5f);
		inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	// Add right turn to the input queue if 'D' is pressed
		if (vehicleInAir) {
			glm::vec3 front = getDir();
			vehiclePtr->getRigidDynamicActor()->addTorque((500.0f + (isFlippedOver() && canFlip ? 10000 : 0)) * PxVec3(front.x, front.y, front.z));
		}
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);			// Add handbrake to the input queue if 'spacebar' is pressed
		state.space_isHeld = true;
		input_handbrake = true;
	}
	else state.space_isHeld = false;



	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !state.shift_isHeld && debugmode != DEBUGMODE::FALSE) {
		glm::vec3 front = getDir();
		vehiclePtr->getRigidDynamicActor()->addForce(10000.0f * PxVec3(front.x, front.y, front.z), physx::PxForceMode::Enum::eIMPULSE);
		state.shift_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		state.shift_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (hasPower) usePower();
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

		GLFWgamepadstate controller_state;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controller_state))
		{
			float forwardOrbackward, leftOrRightturn;
			//forwardOrbackward = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
			leftOrRightturn = controller_state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			
			if (vehicleInAir) {
				glm::vec3 right = getRight();
				vehiclePtr->getRigidDynamicActor()->addTorque(-controller_state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * 1500.0f * PxVec3(right.x, right.y, right.z));
			}

			if (controller_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > -1)
			{
				input_accelerate = true;
				double newSpeed = controller_state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1.0;
				updateSpeed(newSpeed / 2);
				inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed
				//std::cout << "right trigger: " << ControlState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] << std::endl;
			}
			else if (controller_state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > -1)
			{
				state.S_isHeld = true;
				input_reverse = true;
				inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
				//std::cout << "left trigger: " << ControlState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] << std::endl;	//press = 1, idle = -1
			}
			
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE])
			{
				//std::cout << "SQUARE (xbox x, ns pro y)" << std::endl;
				for (Building* b : state.buildings) {
					if (b == nullptr) continue;
					if (b->isInRange) {
						b->triggerFunction(*this, state);
						return;
					}
				}
				state.square_isHeld = true;
			} else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE] == GLFW_RELEASE) state.square_isHeld = false;


			if (leftOrRightturn < -0.15)
			{
				updateLeftSpeed(-leftOrRightturn * 0.5f);
				inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		
				if (vehicleInAir) {
					glm::vec3 back = -getDir();
					vehiclePtr->getRigidDynamicActor()->addTorque((500.0f + (isFlippedOver() && canFlip ? 10000 : 0)) * PxVec3(back.x, back.y, back.z));
				}
			}
			else if (leftOrRightturn > 0.15)
			{
				updateRightSpeed(-leftOrRightturn * 0.5f);
				inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	
				if (vehicleInAir) {
					glm::vec3 front = getDir();
					vehiclePtr->getRigidDynamicActor()->addTorque((500.0f + (isFlippedOver() && canFlip ? 10000 : 0)) * PxVec3(front.x, front.y, front.z));
				}
			}
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER])
			{
				usePower();
			}
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER])
			{
				inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);
				input_handbrake = true;
			}
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS) {
				if (!state.dpad_upisHold) {
					toggleHeadlights = toggleHeadlights + 1 % 3;
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::GEAR_SWITCH0);
				}
				state.dpad_upisHold = true;
			}
			else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_RELEASE) {
				state.dpad_upisHold = false;
			}
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS) {
				if (!state.dpad_downisHold) {
					toggleHeadlights = (toggleHeadlights - 1) % 3;
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::GEAR_SWITCH0);
				}
				state.dpad_downisHold = true;
			}
			else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_RELEASE) {
				state.dpad_downisHold = false;
			}

		}

		

	}
	
	if (timeSpentRobbing > 0) {
		state.audioSystemPtr->stopSound(SOUND_SELECTION::ROB_LOOP);
		state.audioSystemPtr->stopSound(SOUND_SELECTION::OPEN_DUFFLE);
		timeSpentRobbing = 0;
	}
	if (alarmChancePerCheck > baseAlarmChancePerCheck) {
		alarmChancePerCheck -= state.timeStep / (100.f * (float)baseAlarmChancePerCheck);
		if (alarmChancePerCheck < baseAlarmChancePerCheck) alarmChancePerCheck = baseAlarmChancePerCheck;
	}


	footIsOnGas = input_accelerate;
	footIsOnBrake = input_handbrake;
	if (input_reverse) {
		if (getForwardVelocity() > 0) footIsOnBrake = true;
		else {
			footIsOnGas = true;
			isReversing = true;
		}
	}
	else isReversing = false;

}
