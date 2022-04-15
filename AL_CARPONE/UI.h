#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stack>


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

	UI(GraphicsSystem* graphics);
	void update(State* state, Player* player);

private:

	// Graphics
	GraphicsSystem* graphics;
	TextRenderer* text_renderer;

	// POPUPS
	Model* interactPrompt;
	Model* exitPrompt;

	// MINIMAP ELEMENTS
	Model* minimap;
	Model* player_marker;
	Model* police_marker;
	glm::mat4 player_movement = glm::mat4(1.f);

	// Powerups
	std::stack<PowerUp*> pwrups;


	void drawTexts(State* state, Player* player);
	void drawPopups(State* state);
	void drawMinimap(State* state, Player* player);
	glm::mat4 updateMarkerPos(glm::vec3 original_pos);
	glm::vec3 calculateOnMapPos(glm::vec3 pos);
};
