#pragma once

#include "Model.h"

// Make sure to create this after graphics is initialized!!!
class UI {

public:
	Model* press_f_to_rob;

	UI() {
		press_f_to_rob = new Model("models/popups/press_f_to_rob.obj");
	}
};
