#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "GraphicsSystem.h"
#include "Model.h"
#include "State.h"

static class MainMenu {

	unsigned int selectedOption = 0;

public:

	std::vector<Model> level_models;
	std::vector<std::vector<glm::vec3>> level_light_positions;
	std::vector<Model> levels;

	Model* active_level;
	std::vector<glm::vec3>* light_positions;


	MainMenu();
	void changeLevel(int level);
	void drawMenu(GraphicsSystem& graphics, State& state);
	void handleInputs(GLFWwindow* window, State& state);

};