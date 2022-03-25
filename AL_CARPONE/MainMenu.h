#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "AudioSystem.h"
#include "GraphicsSystem.h"
#include "Model.h"
#include "State.h"

enum MenuSelection
{
	PLAYGAME = 0,
	QUIT
};

static class MainMenu {

	unsigned int selectedOption = MenuSelection::PLAYGAME;

public:

	Model loadingMapScreen;
	Model loadingGameScreen;
	Model gameWinScreen;
	Model gameLoseScreen;
	Model jailScreen;


	std::vector<Model> selectionScreens;
	std::vector<std::vector<glm::vec3>> level_light_positions;
	Model level;
	Model garageDoor;
	std::vector<glm::vec3> light_positions;


	MainMenu();
	void drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio);
	void drawLoadingGameScreen(GraphicsSystem& graphics);
	void drawLoadingMapScreen(GraphicsSystem& graphics);
	void drawWinScreen(GraphicsSystem&);	
	void drawJailScreen(GraphicsSystem* graphics);
	void drawLoseScreen(GraphicsSystem&);
	void handleInputs(GLFWwindow* window, State& state, AudioSystem* audio);

};