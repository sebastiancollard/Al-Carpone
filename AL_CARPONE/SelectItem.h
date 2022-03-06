#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "PowerUp.h"
#include "Player.h"
#include "GraphicsSystem.h"
#include "State.h"

static class SelectItem 
{
	unsigned int selection = 0;

public:
	std::vector<Model> Select_Item_Pics;
	
	Model* active_selection;

	SelectItem();
	void changeItem(int item);
	void drawMenu(GraphicsSystem& graphics, State& state);
	void handleInputs(GLFWwindow* window, State& state);
};

