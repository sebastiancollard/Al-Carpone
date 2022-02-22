#pragma once
#include "Ability.h"

enum POWER_TYPE {		//NOTE: did not include radio-jamming as I think we decided this was more of a nice-to-have
	CAMOUFLAGE,
	TOMATO,
	DONUT,
	SPIKE_TRAP
};

class PowerUp : public Ability {
private:
	POWER_TYPE type;		//which type of power-up
	
	float total_duration;		//total duration of the ability (permanent for upgrades?)	//may want to change to time_t depending
								//how we want to implement the timer/ update it per frame
	float timeElapsed;			//total elapsed time of the activated ability

public:
	PowerUp(POWER_TYPE power_type, float duration) {
		type = power_type;
		total_duration = duration;
	}
	void activate();
	void deactivate();
	void update();
	void usePower();

};