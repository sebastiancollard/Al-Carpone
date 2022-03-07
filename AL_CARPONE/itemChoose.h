#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "GraphicsSystem.h"
#include "Model.h"
#include "State.h"
#include "MainMenu.h"

static class itemChoose
{
public:
	// Handle all key inputs relevant to driving
	void mouseDrag(GLFWwindow* window, State& state);
};