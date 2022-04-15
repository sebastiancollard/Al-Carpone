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
		if (!p->isStunned) {
			p->startChase();
		}
	}
}

bool State::policeAlerted() {
	for (PoliceCar* p : activePoliceVehicles) {
		if (p->chaseTime > 0) return true;
	}
	return false;
}

float State::getAlertLevel() {
	float chaseSum = 0;
	float chaseMax = 0;
	for (PoliceCar* p : activePoliceVehicles) {
		chaseSum += p->chaseTime;
		chaseMax += p->maxChaseTime;
	}
	if (chaseMax <= 0) return 0;

	float ratio = chaseSum / chaseMax;

	return ratio;
}

void State::despawnItems()
{
	for (int i = 0; i < active_items.size(); i++)
	{
		if (active_items[i].getRemainingTime() <= 0.f)
		{
			active_items.erase(active_items.begin() + i);	//erase from simple_renderables
			i = i - 1;
		}
	}
}