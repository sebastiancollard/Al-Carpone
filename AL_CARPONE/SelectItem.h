#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "PowerUp.h"
#include "GraphicsSystem.h"
#include "State.h"
#include "AudioSystem.h"

#define TOMATO_PRICE 1000
#define DONUT_PRICE 2000
#define SPIKE_TRAP_PRICE 2500
#define CAMOUFLAG_PRICE 5000

static class SelectItem
{
	unsigned int selection = 0;

public:
	std::vector<Model> Select_Item_Pics;
	std::vector<Model> rotate_item;

	PowerUp powerup;

	Model* active_selection;
	Model* active_rotate;;

	SelectItem();
	void changeItem(int item);
	void drawMenu(GraphicsSystem& graphics, State& state, Player& player);
	void handleInputs(GLFWwindow* window, State& state, Player& player);
};

