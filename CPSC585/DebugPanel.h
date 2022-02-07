#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class DebugPanel {

public:
	
	bool showPanel = false;
	bool updateValues = false;
	ImVec4 clear_color;

	DebugPanel(GLFWwindow* window);
	void cleanUp();

	void draw();
};