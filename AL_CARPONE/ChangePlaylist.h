#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "AudioSystem.h"
#include "GraphicsSystem.h"
#include "Model.h"
#include "State.h"

class ChangePlaylist 
{

	

public:
	
	std::vector<Model> radio_list;

	ChangePlaylist();
	void drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio);
	void handleInputs(GLFWwindow* window, State& state, AudioSystem* audio);
	
};