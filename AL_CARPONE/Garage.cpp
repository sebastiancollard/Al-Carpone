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
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !downPressed && showShop) {
		//std::cout << "down" << std::endl;
		state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
		if (currentSelection < upgradeList.size() - 1) ++currentSelection;
		downPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) downPressed = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !upPressed && showShop) {
		//std::cout << "up" << std::endl;
		state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
		if (currentSelection > 0) --currentSelection;
		upPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) upPressed = false;
	
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !state->f_isHeld) {
		//std::cout << "up" << std::endl;
		state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
		showShop = !showShop;
		state->f_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) state->f_isHeld = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (showShop) {
			state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
			showShop = false;
		}
		state->escape_isHeld = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) state->escape_isHeld = false;

	Upgrade u = upgradeList[currentSelection];
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !enterPressed && showShop) {
		std::cout << "enter" << std::endl;
		if (upgradeList[currentSelection].tier < u.maxTier && player->getCash() >= u.cost(u.tier)) {
			player->setCash(player->getCash() - u.cost(u.tier));
			state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
			state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::UPGRADE);


			switch (u.type) {
			case UPGRADE_TYPE::ROBBERY:
				//if (u.specifier == UPGRADE_SPECIFIER::ROB_SPEED) {
				//	const float upgraded = player->basecashRobbedPerFrame + player->basecashRobbedPerFrame * u.delta(u.tier);
				//	std::cout << "upgrading cash robbed per frame from " << player->cashRobbedPerFrame << " to " << upgraded << std::endl;
				//	player->cashRobbedPerFrame = upgraded;
				//}
				if (u.specifier == UPGRADE_SPECIFIER::ALARM_CHANCE) {
					const float baseChance = player->initAlarmChancePerCheck;
					const float upgraded = baseChance - baseChance * u.delta(u.tier);
					std::cout << "upgrading alarm chance from " << player->baseAlarmChancePerCheck << " to " << upgraded << std::endl;
					player->baseAlarmChancePerCheck = upgraded;
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
				else if (u.specifier == UPGRADE_SPECIFIER::MINIMAP) {
					++player->minimapMode;
					player->drawRadius = u.delta(u.tier);
					std::cout << u.delta(u.tier) << std::endl;
					if (u.tier == 0) std::cout << "police within " << player->drawRadius << "m visible on minimap" << std::endl;
					else std::cout << "All police visible on minimap" << std::endl;
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
					std::cout << "can now recover after flipping upside down" << std::endl;
					player->canFlip = true;
				}
			}

			++player->numUpgradesPurchased;
			if (player->numUpgradesPurchased <= 5) {
				//std::cout << "upgrading bonus bank earnings multiplier from 0x to 1.25x" << std::endl;
				std::cout << "upgrading bank earnings from " << player->cashRobbedPerFrame << " to " << player->cashRobbedPerFrame + player->basecashRobbedPerFrame * .25f << std::endl;
				player->cashRobbedPerFrame += player->basecashRobbedPerFrame * .25f;
			}
			else if (player->numUpgradesPurchased <= 10) {
				//std::cout << "upgrading bonus bank earnings multiplier from 1.25x to 5x" << std::endl;
				std::cout << "upgrading bank earnings from " << player->cashRobbedPerFrame << " to " << player->cashRobbedPerFrame + player->basecashRobbedPerFrame * .75f << std::endl;
				player->cashRobbedPerFrame += player->basecashRobbedPerFrame * 0.75f;
			}
			else if (player->numUpgradesPurchased <= 15) {
				//std::cout << "upgrading bonus bank earnings multiplier from 5x to 20x" << std::endl;
				std::cout << "upgrading bank earnings from " << player->cashRobbedPerFrame << " to " << player->cashRobbedPerFrame + player->basecashRobbedPerFrame * 3.f << std::endl;
				player->cashRobbedPerFrame += player->basecashRobbedPerFrame * 3.f;
			}
			else if (player->numUpgradesPurchased == 24) {
				std::cout << "upgrading bonus bank earnings multiplier from 20x to 80x" << std::endl;
				player->cashRobbedPerFrame = player->basecashRobbedPerFrame * 80.f;
			}
			

			++upgradeList[currentSelection].tier;
		}
		else state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
		//std::cout << upgradeList[currentSelection].tier << std::endl;;
		enterPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) enterPressed = false;



	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		bool accel = false;
		//get controller name
		//const char* controller_name = glfwGetGamepadName(GLFW_JOYSTICK_1);
		//std::cout << controller_name << std::endl;

		GLFWgamepadstate controller_state;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controller_state))
		{
			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS && showShop)
			{
				if (!gpDownPressed) {
					state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_LOW);
					if (currentSelection < upgradeList.size() - 1) ++currentSelection;
				}
				
				gpDownPressed = true;
			}
			else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE] == GLFW_RELEASE) gpDownPressed = false;

			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] && showShop)
			{
				if (!gpUpPressed) {
					state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_CLICK_HIGH);
					if (currentSelection > 0) --currentSelection;
				}
				
				gpUpPressed = true;
			}
			else gpUpPressed = false;

			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE])
			{
				if (!gpSquarePressed) {
					//std::cout << "up" << std::endl;
					state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
					showShop = !showShop;
				}
				gpSquarePressed = true;
			}
			else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE] == GLFW_RELEASE) gpSquarePressed = false;

			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE] && showShop)
			{
				if (!gpCirclePressed) {
					//std::cout << "up" << std::endl;
					state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::MENU_SELECT);
					showShop = false;
				}
				gpCirclePressed = true;
			}
			else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE] == GLFW_RELEASE) gpCirclePressed = false;

			if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_CROSS] && showShop)
			{
				if (!gpXPressed) {
					if (upgradeList[currentSelection].tier < u.maxTier && player->getCash() >= u.cost(u.tier)) {
						player->setCash(player->getCash() - u.cost(u.tier));
						state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
						state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::UPGRADE);
						switch (u.type) {
						case UPGRADE_TYPE::ROBBERY:
							//if (u.specifier == UPGRADE_SPECIFIER::ROB_SPEED) {
							//	const float upgraded = player->basecashRobbedPerFrame + player->basecashRobbedPerFrame * u.delta(u.tier);
							//	std::cout << "upgrading cash robbed per frame from " << player->cashRobbedPerFrame << " to " << upgraded << std::endl;
							//	player->cashRobbedPerFrame = upgraded;
							//}
							if (u.specifier == UPGRADE_SPECIFIER::ALARM_CHANCE) {
								const float baseChance = player->initAlarmChancePerCheck;
								const float upgraded = baseChance - baseChance * u.delta(u.tier);
								std::cout << "upgrading alarm chance from " << player->baseAlarmChancePerCheck << " to " << upgraded << std::endl;
								player->baseAlarmChancePerCheck = upgraded;
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
								for (PoliceCar* p : state->activePoliceVehicles) { 
									p->detectionRadius = upgraded; 
								}
							}
							else if (u.specifier == UPGRADE_SPECIFIER::MINIMAP) {
								++player->minimapMode;
								player->drawRadius = u.delta(u.tier);
								std::cout << u.delta(u.tier) << std::endl;
								if (u.tier == 0) std::cout << "police within " << player->drawRadius << "m visible on minimap" << std::endl;
								else std::cout << "All police visible on minimap" << std::endl;
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
								std::cout << "can now recover after flipping upside down" << std::endl;
								player->canFlip = true;
							}
						}
						++player->numUpgradesPurchased;
						if (player->numUpgradesPurchased <= 5) {
							//std::cout << "upgrading bonus bank earnings multiplier from 0x to 1.25x" << std::endl;
							std::cout << "upgrading bank earnings from " << player->cashRobbedPerFrame << " to " << player->cashRobbedPerFrame + player->basecashRobbedPerFrame * .25f << std::endl;
							player->cashRobbedPerFrame += player->basecashRobbedPerFrame * .25f;
						}
						else if (player->numUpgradesPurchased <= 10) {
							//std::cout << "upgrading bonus bank earnings multiplier from 1.25x to 5x" << std::endl;
							std::cout << "upgrading bank earnings from " << player->cashRobbedPerFrame << " to " << player->cashRobbedPerFrame + player->basecashRobbedPerFrame * .75f << std::endl;
							player->cashRobbedPerFrame += player->basecashRobbedPerFrame * 0.75f;
						}
						else if (player->numUpgradesPurchased <= 15) {
							//std::cout << "upgrading bonus bank earnings multiplier from 5x to 20x" << std::endl;
							std::cout << "upgrading bank earnings from " << player->cashRobbedPerFrame << " to " << player->cashRobbedPerFrame + player->basecashRobbedPerFrame * 3.f << std::endl;
							player->cashRobbedPerFrame += player->basecashRobbedPerFrame * 3.f;
						}
						else if (player->numUpgradesPurchased == 24) {
							std::cout << "upgrading bonus bank earnings multiplier from 20x to 80x" << std::endl;
							player->cashRobbedPerFrame = player->basecashRobbedPerFrame * 80.f;
						}

						++upgradeList[currentSelection].tier;
					}
					else state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);

				}
				
				gpXPressed = true;
			}
			else if (controller_state.buttons[GLFW_GAMEPAD_BUTTON_CROSS] == GLFW_RELEASE) gpXPressed = false;


		}
	}
}

void Garage::drawGarageMenu() {
	graphics.shader2D->use();


	if (!showShop) {
		entrance.Draw(*graphics.shader2D);
		return;
	}

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
	
	
}
