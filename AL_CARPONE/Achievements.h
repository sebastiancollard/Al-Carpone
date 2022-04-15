#pragma once

#include <iostream>
#include "State.h"

class Achievements {

	Achievements();

	bool isFinishGame = false;
	bool isDupeThePolice = false;
	bool isJumpOutOfMap = false;
	bool isRoofOfPoliceStation = false;

	//x: 175.934, y: 0.946, z: -192.472 
	//x: 150.001, y:0.962, z: -204.259
	//x: 117.020, y:0.965, z: -194.216
	bool finishGame(State state);
	bool dupeThePolice();
	bool jumpOutOfMap();
};