#pragma once
#include <chrono>

/*
This class was originally designed to be a game loop timer, but I have abstracted 
some details to make it work as a general timer (ex. to be used with timed power-ups).
https://www.youtube.com/watch?v=Vyt-dM8gTRQ  <---- tutorial this class is based off of
*/


class Timer {
private:
	//static Timer* instance;
	std::chrono::system_clock::time_point startTime;
	std::chrono::duration<float> deltaTime;				//elapsed time (seconds)
	float timescale;

public:

	//static Timer* Instance();
	//static void Release();

	void reset();

	float getDeltaTime();

	float getTimeScale();

	void setTimeScale(float t);

	void tick();
//private:
	Timer();
	~Timer();

};

