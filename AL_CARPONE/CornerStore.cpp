#include "CornerStore.h"
#include "AudioSystem.h"

void CornerStore::createTrigger() {
	trigger = new BoxTrigger(true, tpos, 10.f, 5.f, 10.f);
}

void CornerStore::triggerFunction(Player& player, State& state) {
	player.setChooseTool(true);
	cout << player.canChooseTool(state) << endl;
	state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::ENTER_STORE);
	//state.gamestate = GAMESTATE_CORNERSTORE;
}

