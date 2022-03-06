#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Player.h"

class DebugPanel {

private:
	
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

	// Rages
	const float speed_range[2] = { 0.f, 1.f };
	const float rates_range [2] = { 0.f, 50.f };

public:
	// Functions
	DebugPanel(GLFWwindow* window);
	void cleanUp();

	void draw(Player& player);
};