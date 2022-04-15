#include "UI.h"

#include "Garage.h"
#include "constants.h"



// OBJ paths
#define INTERACT_PATH "models/ui/interact.obj"

#define EXIT_PATH "models/ui/interact_exit.obj"

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
}


// Draw Function Per Frame
void UI::update(State* state, Player* player) {

	// Draw Texts
	drawTexts(state, player);

	graphics->shader2D->use();
	mat4 i = mat4(1.f);

	// Minimap
	drawMinimap(state, player);

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
	else if (state->buildings[BUILDINGS::CORNERSTORE1]->isInRange
		|| state->buildings[BUILDINGS::CORNERSTORE2]->isInRange) {
		interactPrompt->Draw(*graphics->shader2D);
	}

	//Garage Popup
	else if (state->buildings[BUILDINGS::GARAGE1]->isInRange
		|| state->buildings[BUILDINGS::GARAGE2]->isInRange
		|| state->buildings[BUILDINGS::GARAGE3]->isInRange)
	{
		interactPrompt->Draw(*graphics->shader2D);
	}
}



// Texts
void UI::drawTexts(State* state, Player* player) {
	//Render (freeType) text
	Shader& shader = *graphics->shaderText;
	
	// -- CASH -- 
	std::string message = "$" + std::to_string(player->getCash());
	text_renderer->RenderText(*graphics->shaderText, message, 25, SCREEN_HEIGHT - 325, 0.7f);
	//params: shader, text, x_pos(screen coord), y_pos(screen_coord), scale, colour

	// -- CASH MULTIPLIER --
	std::string robSpeed = "$" + std::to_string((int)player->cashRobbedPerFrame) + "/s";
	text_renderer->RenderText(*graphics->shaderText, robSpeed, 25, 75, 0.7f);
	std::string upgradesPurchased = "Upgrades: " + std::to_string((int)player->numUpgradesPurchased) + "/" + std::to_string(24);
	text_renderer->RenderText(*graphics->shaderText, upgradesPurchased, 25, 25, 0.7f);


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
		text_renderer->RenderText(*graphics->shaderText, message, 25.0f, 550.0f, 0.7f);
	}
	int jailCountdown = 5 - (int)player->jailTimer;

	// -- JAIL COUNTDOWN -- 
	if (ratio > 0) {
		message = "Jail Countdown : " + std::to_string(jailCountdown);
		text_renderer->RenderText(*graphics->shaderText, message, 25.0f, 500.0f, 0.7f);
	}

	// -- ALARM RISK
	int num = (int)((float)(player->alarmChancePerCheck - player->baseAlarmChancePerCheck) / (float)(player->baseAlarmChancePerCheck));
	if (num > 0) {
		message = "Alarm Risk : ";
		while (num > 0) {
			message += "+ ";
			num--;
		}
		text_renderer->RenderText(*graphics->shaderText, message, 25.0f, 450.0f, 0.7f);
	}

	// -- EQUIPPED POWERUP --
	PowerUp *pwr = player->getPower();
	if (pwr->getType() != POWER_TYPE::NONE) {
		std::string pwr_str;
		switch (pwr->getType()) {
		case (POWER_TYPE::CAMOUFLAGE):
			pwr_str = "PowerUp: Camouflage";
			break;

		case (POWER_TYPE::TOMATO):
			pwr_str = "PowerUp: Tomato";
			break;

		case (POWER_TYPE::DONUT):
			pwr_str = "PowerUp: Donut";
			break;

		case (POWER_TYPE::SPIKE_TRAP):
			pwr_str = "PowerUp: Spike Trap";
			break;

		default:
			pwr_str = "";
		}
		//message = "Power: " + pwr_str;
		text_renderer->RenderText(*graphics->shaderText, pwr_str, 25, SCREEN_HEIGHT - 375, 0.6f);	//Directly under cash
	}


	// -- LAST POWER TIMER --
	if (pwr->isActive()) {	//only show timer for equipped item
		int rounded = (int) (pwr->getRemainingTime()) + 1;
		message = "Active for " + to_string(rounded) + "s";
		text_renderer->RenderText(*graphics->shaderText, message, 25, SCREEN_HEIGHT - 410, 0.6f);	//Directly under Equipped power
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
	//cout << player_pos.x << ',' << player_pos.y << ',' << player_pos.z << endl;
	return mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 0, 1), vec4(player_pos, 1));
}


// Map in game position to minimap coordinates
glm::vec3 UI::calculateOnMapPos(glm::vec3 pos) {

	float scalex = 0.5 / 1980;
	float scalez = -0.5 / 800;
	float offsetx = -0.955;
	float offsetz = 0.505;

	return vec3(scalex * pos.x + offsetx, scalez * pos.z + offsetz, 0.f);
}