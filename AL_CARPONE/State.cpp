#include "State.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>


State::State() {
	player = new Player();
}


void State::toggleCameraMode() {
	if (cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
		cameraMode = CAMERA_MODE_BOUND;
	}
	else {
		cameraMode = CAMERA_MODE_UNBOUND_FREELOOK;
	}
}

void State::updateTime() {
	currTime = glfwGetTime();
	timeStep = currTime - prevTime;
	prevTime = currTime;
	timeSinceLastFpsUpdate += timeStep;
}