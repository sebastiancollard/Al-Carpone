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
	// main menu
	PLAYGAME = 0,
	OPTIONS,
	QUIT,
	// options menu
	TOGGLE_FULLSCREEN,
	ACHIEVEMENTS,
	CONTROLS,
	BACK_TO_MAIN,
	// controls menu
	KEYBOARD,
	CONTROLLER,
	BACK_TO_OPTIONS,
	// achievements menu
	A_BACK_TO_OPTIONS
};

enum MenuType
{
	MAIN_MENU = 0,
	OPTIONS_MENU,
	CONTROLS_MENU,
	ACHIEVEMENTS_MENU
};

static class MainMenu {

	unsigned int selectedOption = MenuSelection::PLAYGAME;
	unsigned int selectedMenu = MenuType::MAIN_MENU;

	// Menu selection
	void selectUp(State& state, AudioSystem* audio);
	void selectDown(State& state, AudioSystem* audio);
	void selectCurrent(State& state, AudioSystem* audio, GraphicsSystem* graphics);

public:

	Model loadingMapScreen;
	Model loadingGameScreen;
	Model gameWinScreen;
	Model gameLoseScreen;
	Model jailScreen;
	Model checkmark;


	std::vector<Model> selectionScreens;
	std::vector<std::vector<glm::vec3>> level_light_positions;
	Model level;
	Model garageDoor;
	std::vector<glm::vec3> light_positions;


	MainMenu();
	void drawCheckmarks(GraphicsSystem& graphics, State& state);
	void drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio);
	void drawLoadingGameScreen(GraphicsSystem& graphics);
	void drawLoadingMapScreen(GraphicsSystem& graphics);
	void drawWinScreen(GraphicsSystem&);	
	void drawJailScreen(GraphicsSystem* graphics);
	void drawLoseScreen(GraphicsSystem&);
	void handleInputs(State& state, AudioSystem* audio, GraphicsSystem* graphics);

};