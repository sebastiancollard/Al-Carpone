#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "shader.h"
#include "Skybox.h"
#include "State.h"
#include "Player.h"


class GraphicsSystem {

public:

	GLFWwindow* window;
	GLFWmonitor* monitor;
	Skybox* skybox;

	Shader* shader3D;
	Shader* shader2D;
	Shader* shaderText;
	Shader* shaderDetection;

	//Skybox skybox;

	GraphicsSystem();
	void enableDepthBuffer();
	void clearBuffer();
	void swapBuffers();
	void cleanup();
	void toggleFullscreen();

	void updateTitle(State &state, Player& player);

};