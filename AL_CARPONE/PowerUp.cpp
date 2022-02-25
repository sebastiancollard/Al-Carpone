#include "PowerUp.h"

//Constructors
PowerUp::PowerUp(Player p) {
	//type = POWER_TYPE::NONE;
	type = POWER_TYPE::TOMATO;	//just for testing for now
	player = p;
}
PowerUp::PowerUp(POWER_TYPE power_type, Player p) {
	type = power_type;
	player = p;
	duration_sec = 0.f;
}


PowerUp::PowerUp(POWER_TYPE power_type, Player p, float duration) {

	type = power_type;
	player = p;
	duration_sec = duration;
	timer.reset();
}


//public functions
void PowerUp::activateTimed() {
	isActive = true;
	timer.reset();
	if (type == CAMOUFLAGE) {
		player.setDetectable(false);
	}

}

void PowerUp::deactivateTimed() {
	isActive = false;
	if (type == CAMOUFLAGE) {
		player.setDetectable(true);
	}
}

void PowerUp::update() {
	if (duration_sec > 0.f) {
		timer.tick();
		if(timer.getDeltaTime() > duration_sec) {
			deactivate();
		}
	}
}
	
//This should be triggered when the player pressed some "use ability" button for the throwable/placeable items.
void PowerUp::usePower() {
	if (isActive) {
		switch (type) {
		case TOMATO:
			model_path = TOMATO_PATH;
			throw_item = true;
			std::cout << "Using tomato power!" << std::endl;
			break;
		case DONUT:
			//player to throws donut
			break;
		case SPIKE_TRAP:
			//player places spike trap
			break;
		default:
			//Do nothing
			break;
		}
	}
}

std::string PowerUp::getModelPath() {
	return model_path;
}

