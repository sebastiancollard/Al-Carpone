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

	Model loadingMapScreen;
	Model gameWinScreen;
	Model gameLoseScreen;
	Model jailScreen;


	std::vector<Model> selectionScreens;
	std::vector<std::vector<glm::vec3>> level_light_positions;
	std::vector<Model> levels;

	Model* active_level;
	std::vector<glm::vec3>* light_positions;


	MainMenu();
	void changeLevel(int level);
	void drawMenu(GraphicsSystem& graphics, State& state);
	void drawLoadingScreen(GraphicsSystem& graphics);
	void drawWinScreen(GraphicsSystem&);	
	void drawJailScreen(GraphicsSystem* graphics);
	void drawLoseScreen(GraphicsSystem&);
	void handleInputs(GLFWwindow* window, State& state);

};