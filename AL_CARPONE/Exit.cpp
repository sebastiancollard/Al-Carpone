#include "Exit.h"
#include "AudioSystem.h"

void Exit::createTrigger() {
	//ROBBING TRIGGER
	trigger = new BoxTrigger(true, tpos, 80.f, 5.f, 80.f);
}

void Exit::triggerFunction(Player& player, State& state) {
	state.f_isHeld = true;
	state.square_isHeld = true;
	if (player.getCash() >= 250000.f && !state.policeAlerted()) {
		state.gameWon = true;
		state.audioSystemPtr->setMusicVolume(0);
		state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::WINGAME);
		return;
	}
	else state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
}

