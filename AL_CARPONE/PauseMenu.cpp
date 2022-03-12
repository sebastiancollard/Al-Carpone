#include "PauseMenu.h"


PauseMenu::PauseMenu() {

	selectionModels = {
		Model("models/pauseMenu/0_mainMenuSelected.obj"),
		Model("models/pauseMenu/1_optionsSelected.obj")
	};

}

void PauseMenu::drawPauseMenu(GraphicsSystem& graphics, State& state) {
	graphics.shader2D->use();
	selectionModels[selectedOption].Draw(*graphics.shader2D);
}


////////////////////////////////////////////////////////////////////////
// INPUT HANDLING
////////////////////////////////////////////////////////////////////////

//Checks for special inputs that would alter the state, and updates state accordingly
void PauseMenu::handleInputs(GLFWwindow* window, State& state)
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
			if (selectedOption == SELECTION_MAIN_MENU) state.gamestate = GAMESTATE_MAIN_MENU;
			else if (selectedOption == SELECTION_OPTIONS) {} //GO TO OPTIONS MENU
		}
		state.enter_isHeld = true;

		selectedOption = 0;
		return;
	}
	else {
		state.enter_isHeld = false;
	}

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!state.S_isHeld) {
			selectedOption = (selectedOption + 1) % 2;
		}
		state.S_isHeld = true;
		return;
	}
	else state.S_isHeld = false;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			selectedOption = (selectedOption + 1) % 2;
		}
		state.down_isHeld = true;
		return;
	}
	else state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!state.W_isHeld) {

			selectedOption = (selectedOption - 1) % 2;
		}
		state.W_isHeld = true;
		return;
	}
	else state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {

			selectedOption = (selectedOption - 1) % 2;
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
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_START])
			{
				//std::cout << "start (PS4 options, XBOX menu, nspro +) " << std::endl;	//ps4 - options
				if (!state.option_isHeld)
				{
					state.gamestate = GAMESTATE_INGAME;
				}
				state.option_isHeld = true;
				return;
			}
			else
			{
				state.option_isHeld = false;
			}

			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CROSS])
			{
				if (!state.cross_isHeld)
				{
					if (selectedOption == SELECTION_MAIN_MENU) state.gamestate = GAMESTATE_MAIN_MENU;
					else if (selectedOption == SELECTION_OPTIONS) {} //GO TO OPTIONS MENU
				}
				state.cross_isHeld = true;
				return;
				//std::cout << "CROSS (xbox a, ns pro b)" << std::endl;
			}
			else
			{
				state.cross_isHeld = false;
			}

			if ((controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS))
			{
				if (!state.dpad_downisHold) {
					selectedOption = (selectedOption + 1) % 2;
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

					selectedOption = (selectedOption - 1) % 2;
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