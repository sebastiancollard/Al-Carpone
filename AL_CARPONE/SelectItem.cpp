#include "SelectItem.h"


SelectItem::SelectItem() 
{
	Select_Item_Pics = 
	{
		Model("models/ChooseItem/base.obj"),
		Model("models/ChooseItem/camouflag.obj"),
		Model("models/ChooseItem/donut.obj"),
		Model("models/ChooseItem/spike_trap.obj"),
		Model("models/ChooseItem/tomato.obj"),	
	};
	changeItem(0);
}

void SelectItem::changeItem(int item) {
	active_selection = &Select_Item_Pics[item];
}

void SelectItem::drawMenu(GraphicsSystem& graphics, State& state) {
	graphics.shader2D->use();
	//level_models[selectedOption].Draw(*graphics.shader2D);
	active_selection->Draw(*graphics.shader2D);
	handleInputs(graphics.window, state);
}

void SelectItem::handleInputs(GLFWwindow* window, State& state)
{
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!state.down_isHeld) {
			selection = (selection + 1) % 4;
		}
		state.down_isHeld = true;
		return;
	}
	else state.down_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!state.up_isHeld) {

			selection = (selection - 1) % 4;
		}
		state.up_isHeld = true;
		return;
	}
	else state.up_isHeld = false;
}