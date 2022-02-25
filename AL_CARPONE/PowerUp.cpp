#include "PowerUp.h"

//Constructors
PowerUp::PowerUp() {}

PowerUp::PowerUp(POWER_TYPE power_type) {
	type = power_type;
	duration_sec = 0.f;
}


PowerUp::PowerUp(POWER_TYPE power_type, float duration) {
	type = power_type;
	duration_sec = duration;
	timer.reset();
}


//public functions
void PowerUp::activateTimed() {
	activated = true;
	timer.reset();
}

void PowerUp::deactivateTimed() {
	activated = false;
}

void PowerUp::updateTimed() {
	if ((duration_sec > 0.f) && (activated)) {
		timer.tick();
		if(timer.getDeltaTime() > duration_sec) {
			deactivateTimed();
		}
	}
}
	
//This should be triggered when the player pressed some "use ability" button for the throwable/placeable items.
void PowerUp::dropOrThrow() {
	if (!activated) {
		switch (type) {
		case TOMATO:
			model_path = TOMATO_PATH;
			throw_item = true;
			activated = true;
			std::cout << "Using tomato power!" << std::endl;
			break;
		case DONUT:
			//player to throws donut
			model_path = DONUT_PATH;
			throw_item = true;
			activated = true;
			std::cout << "Using doughnut power!" << std::endl;
			break;
		case SPIKE_TRAP:
			model_path = SPIKE_PATH;
			drop_item = true;
			activated = true;
			std::cout << "Using spike power!" << std::endl;
			break;
		default:
			//Do nothing
			break;
		}
	}
}

void PowerUp::stopThrow() {
	throw_item = false;
}

void PowerUp::stopDrop() {
	drop_item = false;
}

POWER_TYPE PowerUp::getType() {
	return type;
}

void PowerUp::setType(POWER_TYPE t) {
	type = t;
	
	duration_sec = 0.f;
	
	activated = false;
	throw_item = false;
	drop_item = false;
}

void PowerUp::setDuration(float d) {
	duration_sec = d;
}

std::string PowerUp::getModelPath() {
	return model_path;
}

