#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Model.h"
#include "State.h"
#include "Player.h"
#include "GraphicsSystem.h"
#include "SelectItem.h"
#include "Garage.h"
#include "TextRenderer.h"


// Make sure to create this after graphics is initialized!!!
class UI {

public:
	TextRenderer* text_renderer;

	UI(GraphicsSystem* graphics);
	void update(State* state, Player* player);

	void showTowPrompt();
	bool shouldTow(State&);
	void resetTowPrompt();

private:

	// Graphics
	GraphicsSystem* graphics;

	// POPUPS
	bool building_activated;
	Model* interactPrompt;
	Model* exitPrompt;
	Model* pwr_tomato;
	Model* pwr_donut;
	Model* pwr_spike;
	Model* pwr_camo;
	Model* towPrompt;
	Model* Item;

	// MINIMAP ELEMENTS
	Model* minimap;
	Model* player_marker;
	Model* police_marker;
	glm::mat4 player_movement = glm::mat4(1.f);

	bool towPromptDisplayed = false;
	float towPromptTimer = 2.0f;


	void drawTexts(State* state, Player* player);
	void drawPopups(State* state);
	void drawPowerups(State* state, Player* player);
	void drawMinimap(State* state, Player* player);
	glm::mat4 updateMarkerPos(glm::vec3 original_pos);
	glm::vec3 calculateOnMapPos(glm::vec3 pos);
};
