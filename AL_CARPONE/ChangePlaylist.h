#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "AudioSystem.h"
#include "GraphicsSystem.h"
#include "Model.h"
#include "State.h"
enum PLAYLIST {
	LIST1,
	LIST2,
	LIST3
};


class ChangePlaylist 
{

	unsigned int currentPlaylist = PLAYLIST::LIST1;

public:
	
	std::vector<Model> radio_list;

	ChangePlaylist();
	void drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio);
	void handleInputs(GLFWwindow* window, State& state, AudioSystem* audio);
	
};