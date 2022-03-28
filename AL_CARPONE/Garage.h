#pragma once
#include<vector>
#include<glm/glm.hpp>
#include "Upgrade.h"
#include "Building.h"
#include "GraphicsSystem.h"

enum GarageTypes {
	ENGINE_GARAGE = 0,
	HANDLING_GARAGE,
	ROBBING_GARAGE
};

enum GarageSelection {
	FIRST = 0,
	SECOND,
	THIRD
};

class Garage : public Building {
public:
	GraphicsSystem& graphics;

	unsigned int type;
	unsigned int currentSelection = 0;

	Model menuTitle;
	Model selectTop;
	Model selectMid;
	Model selectBot;
	Model entrance;

	std::vector<Upgrade> upgradeList;
	physx::PxVec3 position;	// Position currently hardcoded

	float depth = 21.6f;	 //y axis
	float height = 4.f; //z axis
	float width = 21.6;	 //x axis

	bool downPressed = false;
	bool upPressed = false;
	bool enterPressed = false;
	bool fPressed = false;

	bool gpDownPressed = false;
	bool gpUpPressed = false;
	bool gpSquarePressed = false;
	bool gpXPressed = false;
	bool gpCirclePressed = false;

	bool showShop = false;

	Garage(unsigned int upgradeType, PxVec3 pos, PxVec3 dims, GraphicsSystem& g) : graphics(g) { 
		type = upgradeType;
		position = pos;
		createTrigger(dims); 
	}

	void createTrigger(PxVec3 dimensions); // Must only be called once!
	void handleInput(GLFWwindow* window, State* state, Player* player);
	void drawGarageMenu();
	void triggerFunction(Player& player, State& state); // ENTER GARAGE
};