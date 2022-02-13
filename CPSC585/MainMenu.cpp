#include "MainMenu.h"

#include "util.h" //load_positions





MainMenu::MainMenu() {

	level_models.push_back(Model("models/mainMenu/0_tuning_testlevel.obj"));
	level_models.push_back(Model("models/mainMenu/1_racetrack.obj"));
	level_models.push_back(Model("models/mainMenu/2_ai_testlevel.obj"));
	level_models.push_back(Model("models/mainMenu/3_city_scale_testlevel.obj"));

	level_light_positions.push_back(load_positions("models/tuning_testlevel/light_positions.obj"));
	level_light_positions.push_back(load_positions("models/racetrack/light_positions.obj"));
	level_light_positions.push_back(load_positions("models/ai_testlevel/light_positions.obj"));
	level_light_positions.push_back(load_positions("models/city_prototype/light_positions.obj"));
}