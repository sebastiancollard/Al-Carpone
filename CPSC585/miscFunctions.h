#pragma once

//Updates the fps/ms in the window title.
void updateTitle(GLFWwindow* window)
{
	std::string FPS = std::to_string((int)ceil(1. / state.timeStep * state.frameCount));
	std::string RT = std::to_string((state.timeStep / state.frameCount) * 1000);
	std::string title = "Al Carpone / " + FPS + "FPS / " + RT + "ms";
	glfwSetWindowTitle(window, title.c_str());

}
//Checks for special inputs that would alter the state, and updates state accordingly
void checkSpecialInputs(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		state.terminateProgram = true;
		return;
	}
	
	// Handles key inputs
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
			player.reset();
		}
		state.R_isHeld = true;
	}
	else state.R_isHeld = false;


}