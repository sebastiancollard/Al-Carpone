#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "State.h"
#include "Player.h"
#include "AudioSystem.h"


//Checks for special inputs that would alter the state, and updates state accordingly
void checkSpecialInputs(GLFWwindow* window, State& state, Player& player, AudioSystem* audio)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!state.escape_isHeld) {
			state.gamestate = GAMESTATE_PAUSE_MENU;
		}
		state.escape_isHeld = true;
		return;
	}
	else {
		state.escape_isHeld = false;
	}

	
	// Handle any building triggerfunction (rob, get powerup, etc)
	if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)) {
		for (Building* b : state.buildings) {
			if (b == nullptr) continue;
			if (b->isInRange) {
				b->triggerFunction(player, state);
			}
		}
	}


	// Debug Mode
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		if (!state.f5_isHeld) {
			state.debugMode = !state.debugMode;
		}
		state.f5_isHeld = true;
	}
	else state.f5_isHeld = false;

	// Freemode Camera
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (!state.Q_isHeld) {
			state.toggleCameraMode();
		}
		state.Q_isHeld = true;
	}
	else state.Q_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (!state.R_isHeld) {
			state.resetVehicles();
		}
		state.R_isHeld = true;
	}
	else state.R_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		if (!state.M_isHeld) {
			audio->toggleMusic();
		}
		state.M_isHeld = true;
	}
	else state.M_isHeld = false;


	//controller input
	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		//get controller name
		const char* controller_name = glfwGetGamepadName(GLFW_JOYSTICK_1);
		//std::cout << controller_name << std::endl;

		GLFWgamepadstate controller_state;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controller_state))
		{
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_START])
			{
				//std::cout << "start (PS4 options, XBOX menu, nspro +) " << std::endl;	//ps4 - options
				if (!state.option_isHeld) 
				{
					state.gamestate = GAMESTATE_PAUSE_MENU;
				}
				state.option_isHeld = true;
				return;
			}
			else 
			{
				state.option_isHeld = false;
			}
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE])
			{
				//std::cout << "SQUARE (xbox x, ns pro y)" << std::endl;
				for (Building* b : state.buildings) {
					if (b == nullptr) continue;
					if (b->isInRange) {
						b->triggerFunction(player, state);
					}
				}
			}

			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE])
			{
				//std::cout << "TRIANGLE (xbox y, ns pro x)" << std::endl;
				if (!state.triangle_isHeld)
				{
					state.resetVehicles();
				}
				state.R_isHeld = true;
			}
			else state.triangle_isHeld = false;

			
		}
	}
}





