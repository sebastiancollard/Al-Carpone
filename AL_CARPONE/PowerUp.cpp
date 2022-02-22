#include "PowerUp.h"

void PowerUp::activate() {
	isActive = true;
	timeElapsed = 0.f;
	if (type == CAMOUFLAGE) {
		//TODO: render player undetectable to enemies (set some flag?)
	}

}

void PowerUp::deactivate() {
	isActive = false;
}

void PowerUp::update() {
	//TODO: increment time elapsed and deactivate power-up if time is up.
}
	
	
//This should be triggered when the player pressed some "use ability" button for the throwable/placeable items.
void PowerUp::usePower() {
	if (isActive) {
		switch (type) {
		case TOMATO:
			//player to throws tomato
			break;
		case DONUT:
			//player to throws donut
			break;
		case SPIKE_TRAP:
			//player places spike trap
			break;
		}
	}
}
