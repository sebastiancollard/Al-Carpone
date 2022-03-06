#pragma once

#include "Model.h"

// Make sure to create this after graphics is initialized!!!
class UI {

public:
	Model* press_f_to_rob;
	Model* press_f_to_exit;

	BoxTrigger exitTrigger;

	UI() {
		press_f_to_rob = new Model("models/popups/press_f_to_rob.obj");
		press_f_to_exit = new Model("models/popups/press_f_to_exit.obj");
	}

	void update(State* state, Player* player, GraphicsSystem* graphics) {
		//Tell player if they can rob
		if (player->canRob(*state)) {
			graphics->shader2D->use();
			press_f_to_rob->Draw(*graphics->shader2D);
		}

		else if (player->canExit(*state)) {
			graphics->shader2D->use();
			press_f_to_exit->Draw(*graphics->shader2D);
		}
	}
};
