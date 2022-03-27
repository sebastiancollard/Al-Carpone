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
		//std::cout << "down" << std::endl;
		state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		if (currentSelection < upgradeList.size() - 1) ++currentSelection;
		downPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) downPressed = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !upPressed) {
		//std::cout << "up" << std::endl;
		state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
		if (currentSelection > 0) --currentSelection;
		upPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) upPressed = false;
	
	Upgrade u = upgradeList[currentSelection];
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !enterPressed && player->getCash() > u.cost(u.tier)) {
		std::cout << "enter" << std::endl;
		if (upgradeList[currentSelection].tier < u.maxTier && player->getCash() >= u.cost(u.tier)) {
			player->setCash(player->getCash() - u.cost(u.tier));
			state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
			state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::UPGRADE);


			switch (u.type) {
			case UPGRADE_TYPE::ROBBERY:
				if (u.specifier == UPGRADE_SPECIFIER::ROB_SPEED) {
					const float upgraded = player->basecashRobbedPerFrame + player->basecashRobbedPerFrame * u.delta(u.tier);
					std::cout << "upgrading cash robbed per frame from " << player->cashRobbedPerFrame << " to " << upgraded << std::endl;
					player->cashRobbedPerFrame = upgraded;
				}
				else if (u.specifier == UPGRADE_SPECIFIER::DETECTION_RADIUS) {
					float upgraded = 10.f;
					float before = 0.f;
					for (int i = 0; i < state->activePoliceVehicles.size(); i) {
						before = state->activePoliceVehicles[i]->detectionRadius;
						upgraded = state->activePoliceVehicles[i]->baseDetectionRadius - state->activePoliceVehicles[i]->baseDetectionRadius * u.delta(u.tier);
						break;
					}
					std::cout << "upgrading police detection radius from " << before << " to " << upgraded << std::endl;
					for (PoliceCar* p : state->activePoliceVehicles) p->detectionRadius = upgraded;
				}
			case UPGRADE_TYPE::ENGINE:
				if (u.specifier == UPGRADE_SPECIFIER::TOP_SPEED) {
					PxVehicleEngineData t = player->vehiclePtr->mDriveSimData.getEngineData();
					const float upgraded = PLAYER_BASE_ENGINE_PEAK_TORQUE + PLAYER_BASE_ENGINE_PEAK_TORQUE * u.delta(u.tier);
					std::cout << "upgrading peak torque from " << t.mPeakTorque << " to " << upgraded << std::endl;
					t.mPeakTorque = upgraded;
					player->vehiclePtr->mDriveSimData.setEngineData(t);
				}
				else if (u.specifier == UPGRADE_SPECIFIER::GEAR_SPEED) {
					PxVehicleGearsData t = player->vehiclePtr->mDriveSimData.getGearsData();
					const float upgraded = PLAYER_BASE_GEAR_CHANGE_SPEED - PLAYER_BASE_GEAR_CHANGE_SPEED * u.delta(u.tier);
					std::cout << "upgrading gear speed from " << t.mSwitchTime << " to " << upgraded << std::endl;
					t.mSwitchTime = upgraded;
					player->vehiclePtr->mDriveSimData.setGearsData(t);
				}
			case UPGRADE_TYPE::HANDLING:
				if (u.specifier == UPGRADE_SPECIFIER::TIRE_FRICTION) {
					if (timesUpgradedFriction == 0) {
						std::cout << "upgrading road friction from 1x to 1.1x" << std::endl;

						gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) * 1.1f);
						++timesUpgradedFriction;
					}
					else if (timesUpgradedFriction == 1) {
						std::cout << "upgrading road friction from 1.1x to 1.2x" << std::endl;
						gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) / 1.1f);
						gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) * 1.2f);
						++timesUpgradedFriction;
					}
					else if (timesUpgradedFriction == 2) {
						std::cout << "upgrading road friction from 1.2x to 1.3x" << std::endl;
						gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) / 1.2f);
						gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) * 1.3f);
						++timesUpgradedFriction;
					}
				}
				else if (u.specifier == UPGRADE_SPECIFIER::TURN_RADIUS) {
					PxVehicleWheelData t = player->vehiclePtr->mWheelsSimData.getWheelData(0);
					const float upgraded = PLAYER_BASE_STEER_ANGLE + PLAYER_BASE_STEER_ANGLE * u.delta(u.tier);
					std::cout << "upgrading steering angle from " << t.mMaxSteer << " to " << upgraded << std::endl;
					t.mMaxSteer = upgraded;
					player->vehiclePtr->mWheelsSimData.setWheelData(0, t);
					t = player->vehiclePtr->mWheelsSimData.getWheelData(1);
					t.mMaxSteer = upgraded;
					player->vehiclePtr->mWheelsSimData.setWheelData(1, t);
				}
				else if (u.specifier == UPGRADE_SPECIFIER::CAR_FLIP) {
					// TODO
				}
			}



			++upgradeList[currentSelection].tier;
		}
		else state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
		//std::cout << upgradeList[currentSelection].tier << std::endl;;
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
		u.menuElements[u.tier].Draw(*graphics.shader2D);
	}
	// draw indicator of which upgrade you are hovering over
	// handle inputs
}

void Garage::triggerFunction(Player& player, State& state) {
	std::cout << "GARAGE " << this->type << std::endl;
	state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::GARAGE_DOOR);
	drawGarageMenu();
	
}
