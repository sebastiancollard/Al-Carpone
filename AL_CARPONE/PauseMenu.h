#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "GraphicsSystem.h"
#include "Model.h"
#include "State.h"

enum SELECTIONS {
	SELECTION_MAIN_MENU = 0,
	SELECTION_OPTIONS
};

static class PauseMenu {
public:

		PauseMenu();
		void drawPauseMenu(GraphicsSystem& graphics, State& state);
		void handleInputs(GLFWwindow* window, State& state);
	
private:
	unsigned int selectedOption = 0;
	std::vector<Model> selectionModels;

};