#include "Garage.h"
#include "PxPhysicsAPI.h"
#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PxCustomEventCallback.h"

using namespace physx;
using namespace snippetvehicle;


//TODO move to physics system
void Garage::createTrigger() {
	//ROBBING TRIGGER
	PxVec3 t_pos = position;
	trigger = new BoxTrigger(true, t_pos, 10.f, 2.f, 10.f);
}

void Garage::triggerFunction(Player& player, State& state) {
	std::cout << "trigger" << std::endl;
}