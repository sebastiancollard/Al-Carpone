#pragma once

#define CASH_ROBBED_PER_FRAME 5	//$5 per frame for now?

//Checks for special inputs that would alter the state, and updates state accordingly
void checkSpecialInputs(GLFWwindow* window, State& state, Player& player, AudioSystem* audio)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!state.escape_isHeld) {
			state.gamestate = GAMESTATE_PAUSE_MENU;
		}
		state.escape_isHeld = true;
		return;
	}
	else {
		state.escape_isHeld = false;
	}
	
	// Handle bank robbing
	if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) && (player.canRob(state))) {
		//std::cout << "Robbing bank...." << std::endl;
		player.addCash(CASH_ROBBED_PER_FRAME);
	}

	// Debug Mode
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		if (!state.f5_isHeld) {
			state.debugMode = !state.debugMode;
		}
		state.f5_isHeld = true;
	}
	else state.f5_isHeld = false;

	// Freemode Camera
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (!state.Q_isHeld) {
			state.toggleCameraMode();
		}
		state.Q_isHeld = true;
	}
	else state.Q_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (!state.R_isHeld) {
			for (Vehicle* v : state.activeVehicles) {
				v->reset();
			}
		}
		state.R_isHeld = true;
	}
	else state.R_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		if (!state.M_isHeld) {
			audio->toggleMusic();
		}
		state.M_isHeld = true;
	}
	else state.M_isHeld = false;

}





