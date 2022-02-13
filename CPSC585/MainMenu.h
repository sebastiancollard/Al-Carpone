#pragma once
#include<glm/glm.hpp>
#include <vector>
#include "Model.h"

class MainMenu {

public:

	std::vector<Model> level_models;
	std::vector<std::vector<glm::vec3>> level_light_positions;


	MainMenu();


};