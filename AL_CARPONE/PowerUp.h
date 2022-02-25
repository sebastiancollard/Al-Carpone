#pragma once
#include "Ability.h"
#include "Timer.h"

#define TOMATO_PATH "models/powerups/tomato.obj"

enum POWER_TYPE {		//NOTE: did not include radio-jamming as I think we decided this was more of a nice-to-have
	NONE,
	CAMOUFLAGE,
	TOMATO,
	DONUT,
	SPIKE_TRAP
};

class PowerUp : public Ability {
private:
	POWER_TYPE type;		//which type of power-up
	std::string model_path = TOMATO_PATH;	//set to tomato path automatically for testing
	
	Timer timer;
	float duration_sec;		//total duration of the ability (permanent for upgrades?)	
							//If something has a total duration of 0, then it will last infitnitely

public:
	bool throw_item = false;
	bool drop_item = false;

	PowerUp(Player p);
	PowerUp(POWER_TYPE power_type, Player p);
	PowerUp(POWER_TYPE power_type, Player p, float duration);

	void activateTimed();
	void deactivateTimed();
	void update();
	void usePower();
	std::string getModelPath();

};