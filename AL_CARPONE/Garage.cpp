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

void Garage::drawGarageMenu() {
	graphics.shader2D->use();
	menuTitle.Draw(*graphics.shader2D);
	for (Upgrade& u : upgradeList) {
		u.menuElements[0].Draw(*graphics.shader2D);
	}
	// draw indicator of which upgrade you are hovering over
	// handle inputs
}

void Garage::triggerFunction(Player& player, State& state) {
	std::cout << "GARAGE " << this->type << std::endl;
	state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::GARAGE_DOOR);
	drawGarageMenu();
}
