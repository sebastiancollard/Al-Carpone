#include "SelectItem.h"


SelectItem::SelectItem()
{
	Select_Item_Pics =
	{
		Model("models/ChooseItem/tomato.obj"),
		Model("models/ChooseItem/donut.obj"),
		Model("models/ChooseItem/spike_trap.obj"),
		Model("models/ChooseItem/camouflag.obj")
	};

	rotate_item =
	{
		Model("models/powerups/tomato.obj"),
		Model("models/powerups/doughnut.obj"),
		Model("models/powerups/spike.obj"),
		Model("models/powerups/spike.obj")
	};
	changeItem(0);
}

void SelectItem::changeItem(int item) {
	active_selection = new Model("models/ChooseItem/base.obj");
	//active_rotate = new Model("models/powerups/TomatoBeef.obj");
}

void SelectItem::drawMenu(GraphicsSystem& graphics, State& state, Player& player) {
	graphics.shader2D->use();
	Select_Item_Pics[selection].Draw(*graphics.shader2D);

	//graphics.shader3D->use();
	//rotate_item[selection].Draw(*graphics.shader3D);
	//active_rotate->Draw(*graphics.shader3D);
	handleInputs(graphics.window, state, player);
}


//Checks for special inputs that would alter the state, and updates state accordingly
void SelectItem::handleInputs(GLFWwindow* window, State& state, Player& player)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!state.escape_isHeld) {
			state.gamestate = GAMESTATE_INGAME;
		}
		state.escape_isHeld = true;
		return;
	}
	else {
		state.escape_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		if (!state.enter_isHeld) {
			//should select some item
			//std::cout << "select item number:"<< selection << std::endl;
			int cur_cash = player.getCash();
			
			switch (selection)
			{
			case 0:
				if (cur_cash > TOMATO_PRICE ) 
				{
					player.getPower()->setType(TOMATO);
					player.getPower()->setDuration(15.0f);
					player.setCash(cur_cash - TOMATO_PRICE);
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
				}
				else 
				{
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
					return;
				}
					
				break;
			case 1:
				if (cur_cash > DONUT_PRICE)
				{
					player.getPower()->setType(DONUT);
					player.getPower()->setDuration(15.0f);
					player.setCash(cur_cash - DONUT_PRICE);
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
				}
				else
				{
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
					return;
				}
				break;
			case 2:
				if (cur_cash > SPIKE_TRAP_PRICE)
				{
					player.getPower()->setType(SPIKE_TRAP);
					player.getPower()->setDuration(15.0f);
					player.setCash(cur_cash - SPIKE_TRAP_PRICE);
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
				}
				else 
				{
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
					return;
				}
				break;
			case 3:
				if (cur_cash > CAMOUFLAG_PRICE)
				{
					player.getPower()->setType(CAMOUFLAGE);
					player.getPower()->setDuration(15.0f);
					player.setCash(cur_cash - CAMOUFLAG_PRICE);	//1000
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
				}
				else 
				{
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
					return;
				}
				break;
			}
			//cout << player.getPower() << endl;
			state.gamestate = GAMESTATE_INGAME;
			
		}

		state.enter_isHeld = true;

		return;
	}
	else {
		state.enter_isHeld = false;
	}

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		if (!state.S_isHeld) {
			selection = (selection + 1) % 4;
			cout << selection << endl;
			
		}
		state.S_isHeld = true;
		return;
	}
	else state.S_isHeld = false;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			selection = (selection + 1) % 4;
			state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		}
		state.down_isHeld = true;
		return;
	}
	else state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
		if (!state.W_isHeld) {
			//state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
			selection = (selection - 1) % 4;
		}
		state.W_isHeld = true;
		return;
	}
	else state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {
			state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
			selection = (selection - 1) % 4;
		}
		state.up_isHeld = true;
		return;
	}
	else state.up_isHeld = false;

	//this part is for controller input
	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		GLFWgamepadstate controlState;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controlState))
		{
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE])
			{
				if (!state.circle_isHeld) 
				{
					state.gamestate = GAMESTATE_INGAME;
				}
				state.circle_isHeld = true;
				return;
			}
			else 
			{
				state.circle_isHeld = false;
			}

			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CROSS])
			{
				if (!state.cross_isHeld) {
					//should select some item
					//std::cout << "select item number:" << selection << std::endl;
					int cur_cash = player.getCash();
					if (cur_cash < 5) { return; }
					else {
						switch (selection)
						{
						case 0:
							player.getPower()->setType(TOMATO);
							player.getPower()->setDuration(15.0f);
							player.setCash(cur_cash - 5);
							cout << "Tomato 0" << endl;
							break;
						case 1:
							player.getPower()->setType(DONUT);
							player.getPower()->setDuration(15.0f);
							player.setCash(cur_cash - 5);
							cout << "Donut 1" << endl;
							break;
						case 2:
							player.getPower()->setType(SPIKE_TRAP);
							player.getPower()->setDuration(15.0f);
							player.setCash(cur_cash - 5);
							cout << "spike_trap 2" << endl;
							break;
						case 3:
							player.getPower()->setType(CAMOUFLAGE);
							player.getPower()->setDuration(15.0f);
							player.setCash(cur_cash - 5);
							cout << "camouflag 3" << endl;
							break;
						}
						//cout << player.getPower() << endl;
						state.gamestate = GAMESTATE_INGAME;
					}
				}
				//std::cout << "CROSS (xbox a, ns pro b)" << std::endl;
				state.cross_isHeld = true;
				return;
			}
			else {
				state.cross_isHeld = false;
			}

			if ((controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS))
			{
				if (!state.dpad_downisHold) {
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
					selection = (selection + 1) % 4;
				}
				state.dpad_downisHold = true;
				return;
			}
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_RELEASE)
			{
				state.dpad_downisHold = false;
			}

			if ((controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS))
			{
				if (!state.dpad_upisHold) {
					state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
					selection = (selection - 1) % 4;
				}
				state.dpad_upisHold = true;
				return;
			}
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_RELEASE)
			{
				state.dpad_upisHold = false;
			}

		}
	}

}