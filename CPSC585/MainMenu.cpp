#include "MainMenu.h"

#include "util.h" //load_positions





MainMenu::MainMenu() {

	level_models = { 
		Model("models/mainMenu/0_tuning_testlevel.obj"),
		Model("models/mainMenu/1_racetrack.obj"),
		Model("models/mainMenu/2_ai_testlevel.obj"),
		Model("models/mainMenu/3_city_scale_testlevel.obj")
	};

	level_light_positions = {
		load_positions("models/tuning_testlevel/light_positions.obj"),
		load_positions("models/racetrack/light_positions.obj"),
		load_positions("models/ai_testlevel/light_positions.obj"),
		load_positions("models/city_prototype/light_positions.obj")
	};

	levels = {
		Model("models/tuning_testlevel/tuning_testlevel.obj"),
		Model("models/racetrack/racetrack.obj"),
		Model("models/ai_testlevel/ai_testlevel.obj"),
		Model("models/city_prototype/city_prototype.obj")
	};

	changeLevel(0);
}


void MainMenu::changeLevel(int level) {
	active_level = &levels[level];
	light_positions = &level_light_positions[level];
}