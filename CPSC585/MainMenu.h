#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Model.h"

static class MainMenu {

public:

	std::vector<Model> level_models;
	std::vector<std::vector<glm::vec3>> level_light_positions;
	std::vector<Model> levels;
	Model* active_level;


	MainMenu();
	void changeLevel(int level);


};