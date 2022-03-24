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
	//for (Upgrade& u : upgradeList) {
	//	u.menuElements[u.tier].Draw(*graphics.shader2D);
	//}
	// draw indicator of which upgrade you are hovering over
	// handle inputs
	graphics.shader2D->use();
	menuTitle.Draw(*graphics.shader2D);
}

void Garage::triggerFunction(Player& player, State& state) {
	std::cout << "GARAGE " << this->type << std::endl;
	drawGarageMenu();
}
