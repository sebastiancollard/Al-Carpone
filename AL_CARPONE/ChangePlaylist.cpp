#include "ChangePlaylist.h"

ChangePlaylist::ChangePlaylist() {
	radio_list = 
	{
		Model("models/RadioSelect/radio_1.obj"),
		Model("models/RadioSelect/radio_2.obj"),
		Model("models/RadioSelect/radio_3.obj")
	};

}


void ChangePlaylist::drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio) {
	graphics.shader2D->use();
	radio_list[state.playlist_count].Draw(*graphics.shader2D);
	handleInputs(graphics.window, state, audio);
}

void ChangePlaylist::handleInputs(GLFWwindow* window, State& state, AudioSystem* audio)
{
	if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) 
	{
		
			if (!state.right_isHeld) 
			{
				state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
				state.playlist_count = (state.playlist_count + 1) % 3;
			}
			state.right_isHeld = true;
			return;
	}
	else 
	{
		state.right_isHeld = false;
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
	{
		if (!state.left_isHeld)
		{
			state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
			state.playlist_count = (state.playlist_count - 1) % 3;
		}
		state.left_isHeld = true;
		return;
	}
	else
	{
		state.left_isHeld = false;
	}
}