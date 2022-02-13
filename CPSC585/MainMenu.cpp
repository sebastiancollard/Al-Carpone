#include "MainMenu.h"
#include "physXVehicleFunctions.h"
#include "physx_globals.h"
#include "util.h" //load_positions



MainMenu::MainMenu() {

	level_models = { 
		Model("models/mainMenu/0_tuning_testlevel.obj"),
		Model("models/mainMenu/1_racetrack.obj"),
		Model("models/mainMenu/2_ai_testlevel.obj"),
		Model("models/mainMenu/3_city_scale_testlevel.obj")
	};

	level_light_positions = {
		load_positions("models/tuning_testlevel/light_positions.obj"),
		load_positions("models/racetrack/light_positions.obj"),
		load_positions("models/ai_testlevel/light_positions.obj"),
		load_positions("models/city_prototype/light_positions.obj")
	};

	levels = {
		Model("models/tuning_testlevel/tuning_testlevel.obj"),
		Model("models/racetrack/racetrack.obj"),
		Model("models/ai_testlevel/ai_testlevel.obj"),
		Model("models/city_prototype/city_prototype.obj")
	};

	changeLevel(0);
}


void MainMenu::changeLevel(int level) {
	active_level = &levels[level];
	light_positions = &level_light_positions[level];
}



////////////////////////////////////////////////////////////////////////
// DRAW MAIN MENU
////////////////////////////////////////////////////////////////////////


void MainMenu::drawMenu(GraphicsSystem& graphics, State& state) {
	graphics.shader2D->use();
	level_models[selectedOption].Draw(*graphics.shader2D);
	handleInputs(graphics.window, state);

	// Despawn any additional active vehicles (enemies)
	while (state.activeVehicles.size() > 1) {
		despawnEnemy(state.activeVehicles.back());
		state.activeVehicles.pop_back();
	}
}




////////////////////////////////////////////////////////////////////////
// INPUT HANDLING
////////////////////////////////////////////////////////////////////////


//Checks for special inputs that would alter the state, and updates state accordingly
void MainMenu::handleInputs(GLFWwindow* window, State& state)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!state.escape_isHeld) {
			state.terminateProgram = true;
		}
		state.escape_isHeld = true;
		return;
	}
	else {
		state.escape_isHeld = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		state.mainMenu = false;
		state.selectedLevel = selectedOption;
		selectedOption = 0;
		return;
	}

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!state.S_isHeld) {
			selectedOption = (selectedOption + 1) % 4;
		}
		state.S_isHeld = true;
		return;
	}
	else state.S_isHeld = false;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			selectedOption = (selectedOption + 1) % 4;
		}
		state.down_isHeld = true;
		return;
	}
	else state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!state.W_isHeld) {

			selectedOption = (selectedOption - 1) % 4;
		}
		state.W_isHeld = true;
		return;
	}
	else state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {

			selectedOption = (selectedOption - 1) % 4;
		}
		state.up_isHeld = true;
		return;
	}
	else state.up_isHeld = false;

}