#include "MainMenu.h"
#include "physx_globals.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util.h" //load_positions



MainMenu::MainMenu() {

	loadingMapScreen = Model("models/mainMenu/LOADINGMAP.obj");
	loadingGameScreen = Model("models/mainMenu/LOADINGGAME.obj");
	gameWinScreen = Model("models/mainMenu/WINSCREEN.obj");
	gameLoseScreen = Model("models/mainMenu/LOSESCREEN.obj");
	jailScreen = Model("models/mainMenu/BUSTED.obj");
	checkmark = Model("models/mainMenu/checkmark.obj");
	
	selectionScreens = {
		// Main menu
		Model("models/mainMenu/0_PLAYGAME.obj"),
		Model("models/mainMenu/1_OPTIONS.obj"),
		Model("models/mainMenu/2_QUIT.obj"),

		// Options
		Model("models/mainMenu/OPTIONS_togglefullscreen.obj"),
		Model("models/mainMenu/OPTIONS_achievements.obj"),
		Model("models/mainMenu/OPTIONS_controls.obj"),
		Model("models/mainMenu/OPTIONS_back.obj"),

		// Controls
		Model("models/mainMenu/CONTROLS_0_keyboard.obj"),
		Model("models/mainMenu/CONTROLS_1_controller.obj"),
		Model("models/mainMenu/CONTROLS_2_back.obj"),

		// Achievements
		Model("models/mainMenu/ACHIEVEMENTS.obj")
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
	if (selectedOption == MenuSelection::ACHIEVEMENTS) {
		drawCheckmarks(graphics, state);
	}
	handleInputs(state, audio, &graphics);
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

// Where to place checkmark
void MainMenu::drawCheckmarks(GraphicsSystem& graphics, State& state) {

	// Left side
	if (state.isJumpOutOfMap) {
		glm::mat4 pos = glm::translate(glm::mat4(1.f), glm::vec3(-0.8, 0.5, 0));
		graphics.shader2D->setMat4("model", pos);
		checkmark.Draw(*graphics.shader2D);
	}
	if (state.isFinishGame) {
		glm::mat4 pos = glm::translate(glm::mat4(1.f), glm::vec3(-0.8, -0.1, 0));
		graphics.shader2D->setMat4("model", pos);
		checkmark.Draw(*graphics.shader2D);
	}
	if (state.isDupeThePolice) {
		glm::mat4 pos = glm::translate(glm::mat4(1.f), glm::vec3(-0.8, -0.8, 0));
		graphics.shader2D->setMat4("model", pos);
		checkmark.Draw(*graphics.shader2D);
	}
	
	// Right side
	if (state.unlockedAllUpgrades) {
		glm::mat4 pos = glm::translate(glm::mat4(1.f), glm::vec3(0.3, 0.5, 0));
		graphics.shader2D->setMat4("model", pos);
		checkmark.Draw(*graphics.shader2D);
	}
	if (state.isMillionaire) {
		glm::mat4 pos = glm::translate(glm::mat4(1.f), glm::vec3(0.3, -0.1, 0));
		graphics.shader2D->setMat4("model", pos);
		checkmark.Draw(*graphics.shader2D);
	}
	if (state.isRoofOfPoliceStation) {
		glm::mat4 pos = glm::translate(glm::mat4(1.f), glm::vec3(0.3, -0.8, 0));
		graphics.shader2D->setMat4("model", pos);
		checkmark.Draw(*graphics.shader2D);
	}
	graphics.shader2D->setMat4("model", glm::mat4(1.f));
	std::cout << "drawing checkmarks" << std::endl;
}



////////////////////////////////////////////////////////////////////////
// INPUT HANDLING
////////////////////////////////////////////////////////////////////////


void MainMenu::selectUp(State& state, AudioSystem* audio) {

	std::cout << selectedMenu << ", " << selectedOption << std::endl;

	switch (selectedMenu) {

	case MenuType::MAIN_MENU:
		selectedOption = (selectedOption - 1);
		if (selectedOption == -1) selectedOption = 2;
		break;

	case MenuType::OPTIONS_MENU:
		selectedOption = (selectedOption - 1);
		if (selectedOption == 2) selectedOption = 6;
		break;

	case MenuType::CONTROLS_MENU:
		selectedOption = (selectedOption - 1);
		if (selectedOption == 6) selectedOption = 9;
		break;

	case MenuType::ACHIEVEMENTS_MENU:
		selectedOption = MenuSelection::A_BACK_TO_OPTIONS;
		break;
	}

	audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
}


void MainMenu::selectDown(State& state, AudioSystem* audio) {
	switch (selectedMenu) {

	case MenuType::MAIN_MENU:
		selectedOption = (selectedOption + 1) % 3;
		break;

	case MenuType::OPTIONS_MENU:
		selectedOption = (selectedOption + 1);
		if (selectedOption == 7) selectedOption = 3;
		break;

	case MenuType::CONTROLS_MENU:
		selectedOption = (selectedOption + 1);
		if (selectedOption == 10) selectedOption = 7;
		break;

	case MenuType::ACHIEVEMENTS_MENU:
		selectedOption = MenuSelection::A_BACK_TO_OPTIONS;
		break;
	}
	audio->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
}


void MainMenu::selectCurrent(State& state, AudioSystem* audio, GraphicsSystem* graphics) {

	switch (selectedOption) {

	case MenuSelection::PLAYGAME:
		state.gamestate = GAMESTATE_INGAME;
		break;

	case MenuSelection::OPTIONS:
		selectedMenu = MenuType::OPTIONS_MENU;
		selectedOption = MenuSelection::TOGGLE_FULLSCREEN;
		break;

	case MenuSelection::QUIT:
		state.terminateProgram = true;
		break;

	case MenuSelection::TOGGLE_FULLSCREEN:
		graphics->toggleFullscreen();
		break;

	case MenuSelection::ACHIEVEMENTS:
		selectedMenu = MenuType::ACHIEVEMENTS_MENU;
		selectedOption = MenuSelection::A_BACK_TO_OPTIONS;
		break;

	case MenuSelection::CONTROLS:
		selectedMenu = MenuType::CONTROLS_MENU;
		selectedOption = MenuSelection::KEYBOARD;
		break;

	case MenuSelection::BACK_TO_MAIN:
		selectedMenu = MenuType::MAIN_MENU;
		selectedOption = MenuSelection::PLAYGAME;
		break;

	// just show screens (no functionality needed)
	case MenuSelection::KEYBOARD:
	case MenuSelection::CONTROLLER:
		break;
	
	case MenuSelection::BACK_TO_OPTIONS:
		selectedMenu = MenuType::OPTIONS_MENU;
		selectedOption = MenuSelection::BACK_TO_MAIN;
		break;
	
	case MenuSelection::A_BACK_TO_OPTIONS:
		selectedMenu = MenuType::OPTIONS_MENU;
		selectedOption = MenuSelection::BACK_TO_MAIN;
		break;
	}

	audio->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
}




//Checks for special inputs that would alter the state, and updates state accordingly
void MainMenu::handleInputs(State& state, AudioSystem* audio, GraphicsSystem* graphics)
{
	GLFWwindow* window = graphics->window;


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
			selectCurrent(state, audio, graphics);
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
			selectDown(state, audio);
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
			selectDown(state, audio);
		}
		state.down_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!state.W_isHeld) {
			selectUp(state, audio);
		}
		state.W_isHeld = true;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {
			selectUp(state, audio);
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
					selectCurrent(state, audio, graphics);
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
					selectDown(state, audio);
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
					selectUp(state, audio);
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