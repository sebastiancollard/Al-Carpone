#include"State.h"


//Updates the state's time-sensitive variables.
void State::updateTime() {
	currTime = glfwGetTime();
	timeStep = currTime - prevTime;
	prevTime = currTime;
	timeSinceLastFpsUpdate += timeStep;
}

//Toggles the camera mode between bound and unbound.
void State::toggleCameraMode() {
	if (cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
		cameraMode = CAMERA_MODE_BOUND;
	}
	else {
		cameraMode = CAMERA_MODE_UNBOUND_FREELOOK;
	}
}

void State::resetVehicles() {
	for (PoliceCar* p : activePoliceVehicles) {
		p->reset();
	}
	playerPtr->reset();
}

void State::alertPolice() {
	for (PoliceCar* p : activePoliceVehicles) {
		if (!p->isStunned) {
			p->startChase();
		}
	}
}

bool State::policeAlerted() {
	for (PoliceCar* p : activePoliceVehicles) {
		if (p->chaseTime > 0) return true;
	}
	return false;
}

float State::getAlertLevel() {
	float chaseSum = 0;
	float chaseMax = 0;
	for (PoliceCar* p : activePoliceVehicles) {
		chaseSum += p->chaseTime;
		chaseMax += p->maxChaseTime;
	}
	if (chaseMax <= 0) return 0;

	float ratio = chaseSum / chaseMax;

	return ratio;
}

void State::checkAchievements(Player& player) {
	glm::vec3 pos = player.getPos();
	
	//over the police roof
	if (!isRoofOfPoliceStation)
	{
		if ((176 > pos.x) && (pos.x > 117) && (-190 > pos.z) && (pos.z > -205) && (pos.y > 18)) {
			isRoofOfPoliceStation = true;
			cout << "Player made it over the police station roof!" << endl;
		}
	}

	//unlock all upgrade
	if (player.numUpgradesPurchased == 24 && !unlockedAllUpgrades) {
		unlockedAllUpgrades = true;
		cout << "Unlock all upgrade materials!" << endl;
	}

	//Made it out of the map
	if (!isJumpOutOfMap)
	{
		if (pos.y < -21) {
			isJumpOutOfMap = true;
			cout << "Player made it outside of the map!" << endl;
		}
	}

	//Finish Game
	if (!isFinishGame) {
		isFinishGame = gameWon;		//kinda redundant. We dont really need this bool
		if(gameWon)
			cout << "Player has won the game!" << endl;
	}


	//Dupe police
	if (!isDupeThePolice) {
		
	
		//cout << "dupe_level" << dupe_level << "dupe_timer" << dupe_timer << endl;

		
		bool currentlyChased = false;
		

		////cout << " currentlyChased is : " << currentlyChased << endl;
		
		

		 for (PoliceCar* p : activePoliceVehicles) {	// if at least one police car is still chasing the player, make the "overall" state chase
		 	if ((p->ai_state != AISTATE::PATROL)) {
		 		currentlyChased = true;
		 	}
		 }

		 // If the player was previously being chased by at least one police car but is now being chased by none. All police cars must be patrolling (not idle)
		 if (!currentlyChased && previousStateChase) {
		 	isDupeThePolice = true;
		 	cout << "Player has duped the police!" << endl;
		 }

		 if (currentlyChased && dupe_level > 0.9 && dupe_timer == 1)
		 {
			 previousStateChase = true;
		 }
		 else if(!previousStateChase)
		 {
			 previousStateChase = false;
		 }
		 else if (currentlyChased)
		 {

			 previousStateChase = true;
		 }
	}
}