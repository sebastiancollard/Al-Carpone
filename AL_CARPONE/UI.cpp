#include "UI.h"

#include "SelectItem.h"
#include "BoxTrigger.h"
#include "Garage.h"
#include "SelectItem.h"



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
UI::UI() {
	//press_f_to_rob = new Model(ROB_POPUP_PATH);
	//press_f_to_exit = new Model(EXIT_POPUP_PATH);
	//press_f_to_enter_corner_store = new Model(CORNERSTORE_POPUP_PATH);
	//press_f_to_enter_garage = new Model(GARAGE_POPUP_PATH);

	interactPrompt = new Model(INTERACT_PATH);
	exitPrompt = new Model(EXIT_PATH);

	minimap = new Model(MINIMAP_PATH);
	player_marker = new Model(PLAYER_MARKER_PATH);
	police_marker = new Model(POLICE_MARKER_PATH);
	vec3 a = vec3(0.5, -0.5, 0);
	player_marker->meshes[0].vertices[0].Position += a;		// Bottom Left
	police_marker->meshes[0].vertices[0].Position += a;		// Bottom Left
	player_marker->meshes[0].vertices[1].Position += a;		// Bottom Right
	police_marker->meshes[0].vertices[1].Position += a;		// Bottom Right
	player_marker->meshes[0].vertices[2].Position += a;		// Top Right
	police_marker->meshes[0].vertices[2].Position += a;		// Top Right
	player_marker->meshes[0].vertices[3].Position += a;		// Top Left
	police_marker->meshes[0].vertices[3].Position += a;		// Top Left
}


// Draw Function Per Frame
void UI::update(State* state, Player* player, GraphicsSystem* graphics) {

	graphics->shader2D->use();
	mat4 i = mat4(1.f);

	// Draw Minimap
	graphics->shader2D->setMat4("model", mat4(1.f));
	minimap->Draw(*graphics->shader2D);

	// Calculate player marker position
	player_movement = updateMarkerPos(player->getPos());	// Gets translation matrix
	//glm::vec3 out(0, 0, -1);
	//float theta = atan2(glm::dot(glm::cross(player->getDir(), out), glm::vec3(0.f, 1.f, 0.f)), glm::dot(player->getDir(), out));
	//player_movement = glm::rotate(player_movement, theta, out);
	graphics->shader2D->setMat4("model", player_movement);
	player_marker->Draw(*graphics->shader2D);
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
		//if (glm::distance(p->getPos(), player->getPos()) < player->drawRadius) {
		
	}
	if (player->minimapMode == 1) police_marker->Draw(*graphics->shader2D);

	// Draw Popups if applicable
	graphics->shader2D->setMat4("model", mat4(1.f));
	drawPopups(state, graphics);

	/*if (state->g_draw) 
	{
		if (state->buildings[BUILDINGS::GARAGE1]->isInRange) {
			Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE1]));
			g->handleInput(graphics->window, state, player);
			g->drawGarageMenu();
		}
		else if (state->buildings[BUILDINGS::GARAGE2]->isInRange) {
			Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE2]));
			g->handleInput(graphics->window, state, player);
			g->drawGarageMenu();
		}
		else if (state->buildings[BUILDINGS::GARAGE3]->isInRange) {
			Garage* g = ((Garage*)(state->buildings[BUILDINGS::GARAGE3]));
			g->handleInput(graphics->window, state, player);
			g->drawGarageMenu();
		}
	}*/

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
void UI::drawPopups(State* state, GraphicsSystem* graphics) {

	// Tell player if they can rob
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



// Return corresponding translation matrix to pass on to the shader later
mat4 UI::updateMarkerPos(vec3 original_pos) {
	vec3 player_pos = calculateOnMapPos(original_pos);
	//cout << player_pos.x << ',' << player_pos.y << ',' << player_pos.z << endl;
	return mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 0, 1), vec4(player_pos, 1));
}





// Map in game position to minimap coordinates
glm::vec3 UI::calculateOnMapPos(glm::vec3 pos) {

	/* In Case we need to find things dynamically
	// Get Minimap dimensions
	vector<vec3> map_vertices;
	for (auto v : minimap->meshes[0].vertices) {
		map_vertices.push_back(v.Position);
	}

	// LevelMap Dimensions: only (x,z) coords matter
	vector <vec3> game_vertices = {
		vec3(-150, 0, 0),		// Bottom Left
		vec3(930, 0, 0),		// Bottom Right
		vec3(930, 0, -725),		// Top Right
		vec3(-150, 0, -725)		// Top Left
	};
	*/

	float scalex = 0.5 / 1980;
	float scalez = -0.5 / 800;
	float offsetx = -0.955;
	float offsetz = 0.505;
	//float offsetx = 150 * scalex;
	//float offsetz = -725 * scalez;

	return vec3(scalex * pos.x + offsetx, scalez * pos.z + offsetz, 0.f);
}