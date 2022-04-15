#include "MainMenu.h"
#include "physx_globals.h"

#include "util.h" //load_positions



MainMenu::MainMenu() {

	loadingMapScreen = Model("models/mainMenu/LOADINGMAP.obj");
	loadingGameScreen = Model("models/mainMenu/LOADINGGAME.obj");
	gameWinScreen = Model("models/mainMenu/WINSCREEN.obj");
	gameLoseScreen = Model("models/mainMenu/LOSESCREEN.obj");
	jailScreen = Model("models/mainMenu/BUSTED.obj");

	
	selectionScreens = {
		Model("models/mainMenu/0_PLAYGAME.obj"),
		//Model("models/mainMenu/1_OPTIONS.obj"),
		Model("models/mainMenu/2_QUIT.obj")
	};

	light_positions = load_positions("models/map/light_positions.obj");
	
	
	level = Model("models/map/map.obj");
	garageDoor = Model("models/map/garage_door.obj");
}




////////////////////////////////////////////////////////////////////////
// DRAW MAIN MENU
////////////////////////////////////////////////////////////////////////

void MainMenu::drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio) {
	graphics.shader2D->use();
	selectionScreens[selectedOption].Draw(*graphics.shader2D);
	handleInputs(graphics.window, state, audio);
}

void MainMenu::drawLoadingGameScreen(GraphicsSystem& graphics) {
	graphics.shader2D->use();
	loadingGameScreen.Draw(*graphics.shader2D);
}

void MainMenu::drawLoadingMapScreen(GraphicsSystem& graphics) {
	graphics.shader2D->use();
	loadingMapScreen.Draw(*graphics.shader2D);
}

void MainMenu::drawWinScreen(GraphicsSystem& graphics) {
	graphics.shader2D->use();
	gameWinScreen.Draw(*graphics.shader2D);
}

void MainMenu::drawLoseScreen(GraphicsSystem& graphics) {
	graphics.shader2D->use();
	gameLoseScreen.Draw(*graphics.shader2D);
}



void MainMenu::drawJailScreen(GraphicsSystem* graphics) {
	graphics->shader2D->use();
	jailScreen.Draw(*graphics->shader2D);
}

////////////////////////////////////////////////////////////////////////
// INPUT HANDLING
////////////////////////////////////////////////////////////////////////

//Checks for special inputs that would alter the state, and updates state accordingly
void MainMenu::handleInputs(GLFWwindow* window, State& state, AudioSystem* audio)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!state.escape_isHeld) {
			state.gamestate = GAMESTATE_INGAME;
			audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
		}
		state.escape_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		state.escape_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		if (!state.enter_isHeld) {
			if (selectedOption == MenuSelection::PLAYGAME) {
				state.gamestate = GAMESTATE_INGAME;
				audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
				return;
			}
			else if(selectedOption == MenuSelection::QUIT) {
				state.terminateProgram = true;
				audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
				return;
			}			
		}
		state.enter_isHeld = true;

		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
		state.enter_isHeld = false;
	}

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!state.S_isHeld) {
			selectedOption = (selectedOption + 1) % 2;
			audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		}
		state.S_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) state.S_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		if (!state.M_isHeld) {
			audio->toggleMusic();
			audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		}
		state.M_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) state.M_isHeld = false;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			selectedOption = (selectedOption + 1) % 2;
			audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		}
		state.down_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!state.W_isHeld) {

			selectedOption = (selectedOption - 1) % 2;
			audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
		}
		state.W_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {

			selectedOption = (selectedOption - 1) % 2;
			audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);

		}
		state.up_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) state.up_isHeld = false;

	//this part is for controller input
	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		GLFWgamepadstate controlState;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controlState))
		{
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE] == GLFW_PRESS)
			{
				//state.terminateProgram = true;
				state.gamestate = GAMESTATE::GAMESTATE_INGAME;
				audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
				//std::cout << "CIRCLE (xbox b, ns pro a)" << std::endl;
				state.circle_isHeld = true;
				return;
			}
			else if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE] == GLFW_RELEASE) state.circle_isHeld = false;
			
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CROSS] == GLFW_PRESS)
			{
				if (!state.cross_isHeld) 
				{
					if (selectedOption == MenuSelection::PLAYGAME) {
						state.gamestate = GAMESTATE_INGAME;
						audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
						return;
					}
					else if (selectedOption == MenuSelection::QUIT) {
						state.terminateProgram = true;
						audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
						return;
					}
				}
				state.cross_isHeld = true;
				return;
				//std::cout << "CROSS (xbox a, ns pro b)" << std::endl;
			}
			else if (controlState.buttons[GLFW_GAMEPAD_BUTTON_CROSS] == GLFW_RELEASE)
			{
				state.cross_isHeld = false;
			}
			
			if ((controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS))
			{
				if (!state.dpad_downisHold) {
					audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
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
					audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
					selectedOption = (selectedOption - 1) % 2;
				}
				state.dpad_upisHold = true;
				return;
			}
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_RELEASE)
			{
				state.dpad_upisHold = false;
			}
			if (controlState.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
				if (!state.start_isHeld) {
					state.gamestate = GAMESTATE::GAMESTATE_INGAME;
				}
				state.start_isHeld = true;
			}
			else if(controlState.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_RELEASE) {
				state.start_isHeld = false;
			}

		}
	}

}