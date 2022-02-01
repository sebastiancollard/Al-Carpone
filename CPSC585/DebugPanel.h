#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class DebugPanel {

public:

	bool enabled = false;
	bool showPanel = false;
	bool updateValues = false;

	DebugPanel(GLFWwindow* window);
	void cleanUp();

	void draw();
};