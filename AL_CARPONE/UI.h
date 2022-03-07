#pragma once

#include "Model.h"
#include "SelectItem.h"
#include "BoxTrigger.h"

// Make sure to create this after graphics is initialized!!!
class UI {

public:
	Model* press_f_to_rob;
	Model* press_f_to_exit;
	Model* Item;


	UI() {
		press_f_to_rob = new Model("models/popups/press_f_to_rob.obj");
		press_f_to_exit = new Model("models/popups/press_f_to_exit.obj");
		Item = new Model("models/ChooseItem/pressSPACE.obj");
	}

	void update(State* state, Player* player, GraphicsSystem* graphics) {
		//Tell player if they can rob
		if (state->buildings[BUILDINGS::BANK]->isInRange) {
			graphics->shader2D->use();
			press_f_to_rob->Draw(*graphics->shader2D);
		}

		else if (state->buildings[BUILDINGS::EXIT]->isInRange) {
			graphics->shader2D->use();
			press_f_to_exit->Draw(*graphics->shader2D);
		}
		
	}
};
