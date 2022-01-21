class State {
public:
	double currTime = 0.;
	double prevTime = 0.;
	double timeStep;
	unsigned int frameCount = 0;

	void updateTime() {
		currTime = glfwGetTime();
		timeStep = currTime - prevTime;
		++frameCount;
	}

};
