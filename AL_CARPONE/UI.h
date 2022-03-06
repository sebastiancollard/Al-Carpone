#pragma once

#include "Model.h"
#include "SelectItem.h"

// Make sure to create this after graphics is initialized!!!
class UI {

public:
	Model* press_f_to_rob;
	Model* Item;

	UI() {
		press_f_to_rob = new Model("models/popups/press_f_to_rob.obj");
		Item = new Model("models/ChooseItem/pressSPACE.obj");
		//Item = new Model("models/ChooseItem/base.obj");
	}
};
