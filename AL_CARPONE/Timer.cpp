#include "Timer.h"

//Timer* Timer::instance = nullptr;

//Static functions
//Timer* Timer::Instance() {
//	if (instance == nullptr)
//		instance = new Timer();
//	
//	return instance;
//}
//
//void Timer::Release() {
//	delete instance;
//	instance = nullptr;
//}

//Constructor
Timer::Timer() {
	reset();
	timescale = 1.0f;
	deltaTime = std::chrono::duration<float>(0.0f);
}

Timer::Timer(const Timer& t) {
	startTime = t.startTime;
	timescale = t.timescale;
	deltaTime = t.deltaTime;
}

//Destructor
Timer::~Timer() {}

//Other functions
void Timer::reset(){
	startTime = std::chrono::system_clock::now();
}

float Timer::getDeltaTime() {
	return deltaTime.count();
}

void Timer::setTimeScale(float t) {
	timescale = t;
}

float Timer::getTimeScale() {
	return timescale;
}

void Timer::tick() {
	deltaTime = std::chrono::system_clock::now() - startTime;
}