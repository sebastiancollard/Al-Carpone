#include "Garage.h"
#include "PxPhysicsAPI.h"
#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PxCustomEventCallback.h"

using namespace physx;
using namespace snippetvehicle;


//TODO move to physics system
void Garage::createTrigger(PxVec3 dimensions) {
	//ROBBING TRIGGER
	PxVec3 t_pos = position;
	trigger = new BoxTrigger(true, t_pos, dimensions.x, dimensions.y, dimensions.z);
}

void Garage::handleInput(GLFWwindow* window, State* state, Player* player) {
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !downPressed) {
		std::cout << "down" << std::endl;
		if (currentSelection < upgradeList.size() - 1) ++currentSelection;
		downPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) downPressed = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !upPressed) {
		std::cout << "up" << std::endl;
		if (currentSelection > 0) --currentSelection;
		upPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) upPressed = false;
	
	Upgrade u = upgradeList[currentSelection];
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !enterPressed && player->getCash() > u.cost(u.tier)) {
		std::cout << "enter" << std::endl;
		if (upgradeList[currentSelection].tier < u.maxTier) {
			player->setCash(player->getCash() - u.cost(u.tier));
			player->cashRobbedPerFrame = 500 + 500 * u.delta(u.tier);
			++upgradeList[currentSelection].tier;
		} 
		std::cout << upgradeList[currentSelection].tier << std::endl;;
		enterPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) enterPressed = false;
}

void Garage::drawGarageMenu() {
	graphics.shader2D->use();
	menuTitle.Draw(*graphics.shader2D);
	switch (currentSelection) {
		case (GarageSelection::FIRST):
			selectTop.Draw(*graphics.shader2D);
			break;
		case (GarageSelection::SECOND):
			selectMid.Draw(*graphics.shader2D);
			break;
		case (GarageSelection::THIRD):
			selectBot.Draw(*graphics.shader2D);
			break;
	}
	for (Upgrade& u : upgradeList) {
		u.menuElements[0].Draw(*graphics.shader2D);
	}
	// draw indicator of which upgrade you are hovering over
	// handle inputs
}

void Garage::triggerFunction(Player& player, State& state) {
	std::cout << "GARAGE " << this->type << std::endl;
	drawGarageMenu();
}
