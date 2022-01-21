#define CAMERA_MODE_BOUND 0
#define CAMERA_MODE_BOUND_FREELOOK 1
#define CAMERA_MODE_UNBOUND_FREELOOK 2


class State {
public:
	double currTime = 0.;
	double prevTime = 0.;
	double timeStep;

	unsigned int frameCount = 0;
	unsigned int cameraMode = CAMERA_MODE_BOUND;

	bool Q_isHeld = false;

	void updateTime() {
		currTime = glfwGetTime();
		timeStep = currTime - prevTime;
		++frameCount;
	}

	void toggleCameraMode() {
		if (cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) cameraMode = CAMERA_MODE_BOUND;
		else cameraMode = CAMERA_MODE_UNBOUND_FREELOOK;

	}

};
