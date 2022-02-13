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
	Skybox* skybox;

	std::shared_ptr<Shader> shader3D;
	std::shared_ptr<Shader> shader2D;

	//Skybox skybox;

	GraphicsSystem();
	void enableDepthBuffer();
	void clearBuffer();
	void swapBuffers();
	void cleanup();

	void updateTitle(State &state, Player& player);

};