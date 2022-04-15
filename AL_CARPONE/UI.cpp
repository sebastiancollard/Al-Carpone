#include "UI.h"

#include "Garage.h"
#include "constants.h"



// OBJ paths
#define INTERACT_PATH "models/ui/interact.obj"
#define EXIT_PATH "models/ui/interact_exit.obj"
#define TOW_PATH "models/ui/towPrompt.obj"

#define TOMATO_UI_PATH "models/ui/tomato_ui.obj"
#define DONUT_UI_PATH "models/ui/donut_ui.obj"
#define SPIKE_UI_PATH "models/ui/spike_ui.obj"
#define CAMO_UI_PATH "models/ui/camouflage_ui.obj"

// MINIMAP paths
#define MINIMAP_PATH "models/ui/minimap.obj"
#define PLAYER_MARKER_PATH "models/ui/alcarpone_marker.obj"
#define POLICE_MARKER_PATH "models/ui/police_marker.obj"



using namespace std;
using namespace glm;



// Initializer
UI::UI(GraphicsSystem* g) : graphics(g) {

	cout << "	FreeType..." << endl;
	text_renderer = new TextRenderer();
	text_renderer->initFont();

	interactPrompt = new Model(INTERACT_PATH);
	exitPrompt = new Model(EXIT_PATH);
	towPrompt = new Model(TOW_PATH);

	pwr_tomato = new Model(TOMATO_UI_PATH);
	pwr_donut = new Model(DONUT_UI_PATH);
	pwr_spike = new Model(SPIKE_UI_PATH);
	pwr_camo = new Model(CAMO_UI_PATH);

	minimap = new Model(MINIMAP_PATH);
	player_marker = new Model(PLAYER_MARKER_PATH);
	police_marker = new Model(POLICE_MARKER_PATH);

	vec3 a = vec3(0.5, -0.5, 0);
	for (auto marker : { player_marker, police_marker }) {
		marker->meshes[0].vertices[0].Position += a;		// Bottom Left
		marker->meshes[0].vertices[1].Position += a;		// Bottom Right
		marker->meshes[0].vertices[2].Position += a;		// Top Right
		marker->meshes[0].vertices[3].Position += a;		// Top Left
	}

	for (int i = 0; i < 10; i++) timer.tick();
}


// Draw Function Per Frame
void UI::update(State* state, Player* player) {

	// Texts must be drawn before 2D images!
	drawTexts(state, player);

	graphics->shader2D->use();

	// UI elements
	drawMinimap(state, player);
	drawPowerups(state, player);

	// Draw Popups if applicable
	graphics->shader2D->setMat4("model", mat4(1.f));
	drawPopups(state);
	

	// Garages / CornerStores
	if (state->buildings[BUILDINGS::GARAGE1]->isInRange) {
		Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE1]));
		g->handleInput(graphics->window, state, player);
		g->drawGarageMenu();
	}
	else {
		Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE1]));
		g->showShop = false;
	}

	if (state->buildings[BUILDINGS::GARAGE2]->isInRange) {
		Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE2]));
		g->handleInput(graphics->window, state, player);
		g->drawGarageMenu();
	}
	else {
		Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE2]));
		g->showShop = false;
	}

	if (state->buildings[BUILDINGS::GARAGE3]->isInRange) {
		Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE3]));
		g->handleInput(graphics->window, state, player);
		g->drawGarageMenu();
	}
	else {
		Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE3]));
		g->showShop = false;
	}



	if (towPromptDisplayed) {
		towPrompt->Draw(*graphics->shader2D);
		towPromptTimer -= state->timeStep;

		if (towPromptTimer <= 0) {
			towPromptTimer = 2.0f;
			towPromptDisplayed = false;
		}
	}

}



// Draw All Popup items
void UI::drawPopups(State* state) {

	// Bank Popup
	if (state->buildings[BUILDINGS::BANK]->isInRange) {
		interactPrompt->Draw(*graphics->shader2D);
	}

	// Exit Popup
	else if (state->buildings[BUILDINGS::EXIT]->isInRange) {
		exitPrompt->Draw(*graphics->shader2D);
	}

	// Cornerstore Popup
	if (state->buildings[BUILDINGS::CORNERSTORE1]->isInRange
		|| state->buildings[BUILDINGS::CORNERSTORE2]->isInRange) {
		interactPrompt->Draw(*graphics->shader2D);
		building_activated = true;
	}

	//Garage Popup
	else if (state->buildings[BUILDINGS::GARAGE1]->isInRange
		|| state->buildings[BUILDINGS::GARAGE2]->isInRange
		|| state->buildings[BUILDINGS::GARAGE3]->isInRange)
	{
		interactPrompt->Draw(*graphics->shader2D);
		building_activated = true;
	}

	else building_activated = false;
}



// Texts
void UI::drawTexts(State* state, Player* player) {
	//Render (freeType) text
	Shader& shader = *graphics->shaderText;
	
	// -- CASH -- 
	std::string message = "$" + std::to_string(player->getCash());
	text_renderer->RenderText(shader, message, 25, SCREEN_HEIGHT - 325, 0.7f);
	//params: shader, text, x_pos(screen coord), y_pos(screen_coord), scale, colour

	// -- CASH MULTIPLIER --
	std::string robSpeed = "$" + std::to_string((int)player->cashRobbedPerFrame) + "/s";
	text_renderer->RenderText(shader, robSpeed, 25, 75, 0.7f);
	std::string upgradesPurchased = "Upgrades: " + std::to_string((int)player->numUpgradesPurchased) + "/" + std::to_string(24);
	text_renderer->RenderText(shader, upgradesPurchased, 25, 25, 0.7f);


	// -- ALERT LEVEL --
	float chaseSum = 0;
	float chaseMax = 0;

	for (PoliceCar* p : state->activePoliceVehicles) {
		chaseSum += p->chaseTime;
		chaseMax += p->maxChaseTime;
	}

	float ratio = chaseSum / chaseMax;

	message = "Alert Level : ";

	if (ratio > 0) {
		if (ratio > 0)   message += "* ";
		if (ratio > 0.2) message += "* ";
		if (ratio > 0.4) message += "* ";
		if (ratio > 0.6) message += "* ";
		if (ratio > 0.8) message += "* ";
		text_renderer->RenderText(shader, message, 25.0f, 550.0f, 0.7f);
	}
	int jailCountdown = 5 - (int)player->jailTimer;

	// -- JAIL COUNTDOWN -- 
	if (ratio > 0) {
		message = "Jail Countdown : " + std::to_string(jailCountdown);
		text_renderer->RenderText(shader, message, 25.0f, 500.0f, 0.7f);
	}

	state->dupe_level = ratio;
	state->dupe_timer = jailCountdown;

	

	// -- ALARM RISK
	int num = (int)((float)(player->alarmChancePerCheck - player->baseAlarmChancePerCheck) / (float)(player->baseAlarmChancePerCheck));
	if (num > 0) {
		message = "Alarm Risk : ";
		while (num > 0) {
			message += "+ ";
			num--;
		}
		text_renderer->RenderText(shader, message, 25.0f, 450.0f, 0.7f);
	}


	// -- Achievements
	if (state->newAchievement) {
		timer.reset();
		state->newAchievement = false;
		timer.tick();
	}
	if (timer.getDeltaTime() < 5.f) {
		message = "* New Achievement Unlocked! *";
		text_renderer->RenderText(shader, message, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 50, 0.7f);
		timer.tick();
	}


	// -- Powerup timer
	for (int i = 0; i < state->active_items.size(); i++) {

		float width = SCREEN_WIDTH - SCREEN_WIDTH / 7;
		float height = 25  + (i * 110);

		if (player->hasPower) height += 110;

		int rounded = (int)(state->active_items[i].getRemainingTime()) + 1;
		message = "Active for " + to_string(rounded) + "s";
		text_renderer->RenderText(*graphics->shaderText, message, width, height, 0.6f);	// Directly under
	}
}


void UI::drawPowerups(State* state, Player* player) {

	float offset = 0;
	
	// Draw currently holding power
	if (player->hasPower) {

		PowerUp* pwr = player->getPower();
		Model* m;

		switch (pwr->getType()) {
		case (POWER_TYPE::CAMOUFLAGE):
			m = pwr_camo;
			break;

		case (POWER_TYPE::TOMATO):
			m = pwr_tomato;
			break;

		case (POWER_TYPE::DONUT):
			m = pwr_donut;
			break;

		default:
			m = pwr_spike;
			break;
		}

		mat4 pos = translate(mat4(1.f), vec3(0.94, -0.9, 0));
		graphics->shader2D->setMat4("model", pos);
		m->Draw(*graphics->shader2D);

		offset = 0.2;
	}

	// Draw list of active powerups
	for (int i = 0; i < state->active_items.size(); i++) {

		Model* m;

		switch (state->active_items[i].getType()) {
			case (POWER_TYPE::CAMOUFLAGE):
				m = pwr_camo;
				break;

			case (POWER_TYPE::TOMATO):
				m = pwr_tomato;
				break;

			case (POWER_TYPE::DONUT):
				m = pwr_donut;
				break;

			default:
				m = pwr_spike;
				break;
		}

		mat4 pos = translate(mat4(1.f), vec3(0.94,-0.9 + i*0.2 + offset, 0));
		graphics->shader2D->setMat4("model", pos);
		m->Draw(*graphics->shader2D);
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MINIMAP MARKER POSITIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UI::drawMinimap(State* state, Player* player) {
	// Draw Minimap
	graphics->shader2D->setMat4("model", mat4(1.f));
	minimap->Draw(*graphics->shader2D);

	// Calculate player marker position
	player_movement = updateMarkerPos(player->getPos());	// Gets translation matrix
	graphics->shader2D->setMat4("model", player_movement);
	player_marker->Draw(*graphics->shader2D);

	// Calculate police marker position(s)
	float closest = 9999.f;
	for (PoliceCar* p : state->activePoliceVehicles) {
		switch (player->minimapMode) {
		case 1:
			if (glm::distance(p->getPos(), player->getPos()) < closest) {
				closest = glm::distance(p->getPos(), player->getPos());
				player_movement = updateMarkerPos(p->getPos());	// Gets translation matrix
				graphics->shader2D->setMat4("model", player_movement);
			}
			break;
		case 2:
			if (glm::distance(p->getPos(), player->getPos()) < player->drawRadius) {
				player_movement = updateMarkerPos(p->getPos());	// Gets translation matrix
				graphics->shader2D->setMat4("model", player_movement);
				police_marker->Draw(*graphics->shader2D);
			}
			break;
		case 3:
			player_movement = updateMarkerPos(p->getPos());	// Gets translation matrix
			graphics->shader2D->setMat4("model", player_movement);
			police_marker->Draw(*graphics->shader2D);
			break;
		}
	}
	if (player->minimapMode == 1) police_marker->Draw(*graphics->shader2D);
}


// Return corresponding translation matrix to pass on to the shader later
mat4 UI::updateMarkerPos(vec3 original_pos) {
	vec3 player_pos = calculateOnMapPos(original_pos);
	return translate(mat4(1.f), player_pos);
}


// Map in game position to minimap coordinates
glm::vec3 UI::calculateOnMapPos(glm::vec3 pos) {

	float scalex = 0.5 / 1980;
	float scalez = -0.5 / 800;
	float offsetx = -0.955;
	float offsetz = 0.505;

	return vec3(scalex * pos.x + offsetx, scalez * pos.z + offsetz, 0.f);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TOWING
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UI::showTowPrompt() {
	towPromptDisplayed = true;
}
bool UI::shouldTow(State& state) {
	return towPromptDisplayed && !state.policeAlerted();
}
void UI::resetTowPrompt() {
	towPromptDisplayed = false;
	towPromptTimer = 2.f;
}