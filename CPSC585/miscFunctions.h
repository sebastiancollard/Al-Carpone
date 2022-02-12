#pragma once


//Checks for special inputs that would alter the state, and updates state accordingly
void checkMainMenuInputs(GLFWwindow* window)
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
		state.selectedLevel = state.selectedMainMenuOption;
		state.selectedMainMenuOption = 0;
		return;
	}

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!state.S_isHeld) {
			state.selectedMainMenuOption = (state.selectedMainMenuOption + 1) % 4;
		}
		state.S_isHeld = true;
		return;
	}
	else state.S_isHeld = false;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			state.selectedMainMenuOption = (state.selectedMainMenuOption + 1) % 4;
		}
		state.down_isHeld = true;
		return;
	}
	else state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!state.W_isHeld) {

			state.selectedMainMenuOption = (state.selectedMainMenuOption - 1) % 4;
		}
		state.W_isHeld = true;
		return;
	}
	else state.W_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {

			state.selectedMainMenuOption = (state.selectedMainMenuOption - 1) % 4;
		}
		state.up_isHeld = true;
		return;
	}
	else state.up_isHeld = false;

}
#define CASH_ROBBED_PER_FRAME 5	//$5 per frame for now?

//Checks for special inputs that would alter the state, and updates state accordingly
void checkSpecialInputs(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!state.escape_isHeld) {
			state.mainMenu = true;
		}
		state.escape_isHeld = true;
		return;
	}
	else {
		state.escape_isHeld = false;
	}
	
	// Handle bank robbing
	if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) && (player.canRob())) {
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
			for (Vehicle* v : activeVehicles) {
				v->reset();
			}
		}
		state.R_isHeld = true;
	}
	else state.R_isHeld = false;


}


std::vector<glm::vec3> load_positions(std::string path) {
	std::vector<std::string> lines;
	std::string line;

	std::vector<glm::vec3> positions;

	std::ifstream input(path);

	if (!input.is_open()) {
		printf("Error opening file: %s\n", path.c_str());
		return positions;
	}

	while (getline(input, line)) if (line[0] == 'v' && line[1] == ' ') lines.push_back(line.substr(2));

	for (std::string l : lines) {
		std::vector<float> positions_raw;
		std::string numberStr;

		for (char c : l) {
			if (c == ' ' && numberStr.size() > 0) {
				float num;
				stringstream(numberStr) >> num;

				positions_raw.push_back(num);
				numberStr.clear();
			}
			else numberStr.push_back(c);
		}
		float num;
		stringstream(numberStr) >> num;

		positions_raw.push_back(num);
		numberStr.clear();

		positions.push_back(glm::vec3(positions_raw[0], positions_raw[1], positions_raw[2]));
		positions_raw.clear();
	}

	return positions;
}


void despawnEnemy(Vehicle* enemy) {
	gScene->removeActor(*enemy->actorPtr);
	for (int i = 0; i < physx_actors.size(); i++) {
		if (physx_actors[i].actorPtr == enemy->actorPtr) {
			physx_actors.erase(physx_actors.begin() + i);
			return;
		}
	}
	delete(enemy);
}
