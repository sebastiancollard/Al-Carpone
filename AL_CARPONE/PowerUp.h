#pragma once
#include "Timer.h"
#include <iostream>

#define TOMATO_PATH "models/powerups/tomato.obj"
#define DONUT_PATH "models/powerups/doughnut.obj"		//TO CHANGE
#define SPIKE_PATH "models/powerups/spike.obj"

enum POWER_TYPE {		//NOTE: did not include radio-jamming as I think we decided this was more of a nice-to-have
	NONE,
	CAMOUFLAGE,
	TOMATO,
	DONUT,
	SPIKE_TRAP
};

class PowerUp {
private:
	POWER_TYPE type = SPIKE_TRAP;		//which type of power-up
	std::string model_path;			//set to tomato path automatically for testing
	
	Timer timer;			
	float duration_sec = 0.f;		//total duration of the ability (permanent for upgrades?)	
									//If something has a total duration of 0, then it will last infitnitely
	bool activated = false;			//affects timed abilities, but also checks to make sure we only throw one donut, etc. at a time

public:
	bool throw_item = false;
	bool drop_item = false;

	PowerUp();
	PowerUp(POWER_TYPE power_type);
	PowerUp(POWER_TYPE power_type, float duration);

	void activateTimed();			//resets timer for ability and sets isActive to true
	void deactivateTimed();			//sets isActive to false
	void updateTimed();				//If there is a timed ability, make a tick on the timer and deactivate the ability when time is up
	void dropOrThrow();				//Set flags necessary to drop/throw the appropriate item
	
	POWER_TYPE getType();
	void setType(POWER_TYPE t);		//resets appropriate variables and changes the power type
	void setDuration(float d);		//sets the timer duration if it needs to be changed
	void stopThrow();
	void stopDrop();
	std::string getModelPath();

};