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

	// Color (currently unused)
	ImVec4 color;

	// Steer Vs Forward Speed Data
	float* speed0;
	float* speed5;
	float* speed30;
	float* speed120;

	// KeySmoothing Data
	float* key_riseRates[5];
	float* key_fallRates[5];

	// PadSmoothing Data
	float* pad_riseRates[5];
	float* pad_fallRates[5];


	// Functions
	DebugPanel(GLFWwindow* window);
	void cleanUp();

	void draw();
};