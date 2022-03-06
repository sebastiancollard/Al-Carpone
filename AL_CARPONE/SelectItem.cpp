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
	changeItem(0);
}

void SelectItem::changeItem(int item) {
	active_selection = new Model("models/ChooseItem/base.obj");
}

void SelectItem::drawMenu(GraphicsSystem& graphics, State& state) {
	graphics.shader2D->use();
	Select_Item_Pics[selection].Draw(*graphics.shader2D);
	//active_selection->Draw(*graphics.shader2D);
	handleInputs(graphics.window, state);
}


//Checks for special inputs that would alter the state, and updates state accordingly
void SelectItem::handleInputs(GLFWwindow* window, State& state)
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
			std::cout << "item select" << std::endl;
		}

		state.enter_isHeld = true;

		return;
	}
	else {
		state.enter_isHeld = false;
	}

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!state.S_isHeld) {
			selection = (selection + 1) % 4;
		}
		state.S_isHeld = true;
		return;
	}
	else state.S_isHeld = false;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			selection = (selection + 1) % 4;
		}
		state.down_isHeld = true;
		return;
	}
	else state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!state.W_isHeld) {

			selection = (selection - 1) % 4;
		}
		state.W_isHeld = true;
		return;
	}
	else state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {

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
				state.terminateProgram = true;
				//std::cout << "CIRCLE (xbox b, ns pro a)" << std::endl;
				return;
			}

			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CROSS])
			{
				state.gamestate = GAMESTATE::GAMESTATE_MAIN_MENU;
				state.selectedLevel = selection;
				selection = 0;
				return;
				//std::cout << "CROSS (xbox a, ns pro b)" << std::endl;
			}

			if ((controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS))
			{
				if (!state.dpad_downisHold) {

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