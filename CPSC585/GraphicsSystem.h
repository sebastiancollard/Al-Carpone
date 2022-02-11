#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "shader.h"
#include "Skybox.h"


class GraphicsSystem {

public:

	GLFWwindow* window;

	std::shared_ptr<Shader> shader3D;
	std::shared_ptr<Shader> shader2D;

	//Skybox skybox;

	GraphicsSystem();
	void clearBuffer();
	void swapBuffers();
	void cleanup();

};