#include"State.h"


//Updates the state's time-sensitive variables.
void State::updateTime() {
	currTime = glfwGetTime();
	timeStep = currTime - prevTime;
	prevTime = currTime;
	timeSinceLastFpsUpdate += timeStep;
}

//Toggles the camera mode between bound and unbound.
void State::toggleCameraMode() {
	if (cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
		cameraMode = CAMERA_MODE_BOUND;
	}
	else {
		cameraMode = CAMERA_MODE_UNBOUND_FREELOOK;
	}
}

void State::resetVehicles() {
	for (PoliceCar* p : activePoliceVehicles) {
		p->reset();
	}
	playerPtr->reset();
}

void State::alertPolice() {
	for (PoliceCar* p : activePoliceVehicles) {
		p->startChase();
	}
}

bool State::policeAlerted() {
	for (PoliceCar* p : activePoliceVehicles) {
		if (p->chaseTime > 0) return true;
	}
	return false;
}