#include "Exit.h"

void Exit::createTrigger() {
	//ROBBING TRIGGER
	PxVec3 t_pos = b_pos;
	trigger = new BoxTrigger(true, t_pos, 80.f, 5.f, 80.f);
}

void Exit::triggerFunction(Player& player, State& state) {
	
	

	if (player.getCash() > 100.f) {
		state.gameWon = true;
		return;
	}
}

