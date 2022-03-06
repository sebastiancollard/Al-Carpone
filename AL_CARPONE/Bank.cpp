#include "Bank.h"
#include "PxPhysicsAPI.h"
#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PxCustomEventCallback.h"


#define CASH_ROBBED_PER_FRAME 5		// $5 per frame for now?

using namespace physx;
using namespace snippetvehicle;


//TODO move to physics system
void Bank::createTrigger() {
	//ROBBING TRIGGER
	PxVec3 t_pos = b_pos;
	trigger = new BoxTrigger(true, t_pos, 80.f, 5.f, 80.f);
}

void Bank::triggerFunction(Player& player, State& state){
	player.addCash((double) CASH_ROBBED_PER_FRAME * state.timeStep);
}