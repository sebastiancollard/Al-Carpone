#include "BoxTrigger.h"
#include "PxPhysicsAPI.h"
#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PxCustomEventCallback.h"


using namespace physx;
using namespace snippetvehicle;




BoxTrigger::BoxTrigger(bool is_static, float width, float height, float depth, PxVec3 t_pos) {

	// TODO: for now, capsule is not handled
	// 
	// BOX = (a/2, b/2, c/2) where a,b,c are the sides of the box
	triggerShape = gPhysics->createShape(PxBoxGeometry(PxVec3(width/2, height/2, depth/2)), *gMaterial);
	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);				//This is a trigger shape.

	// Filters
	PxFilterData triggerFilter(COLLISION_FLAG_TRIGGER, COLLISION_FLAG_TRIGGER_AGAINST, 0, 0);
	triggerShape->setSimulationFilterData(triggerFilter);

	pos = t_pos;
	PxTransform triggerPos(pos);											// position of the trigger.

	if (is_static) {
		PxRigidStatic* triggerBody = gPhysics->createRigidStatic(triggerPos);
		triggerBody->attachShape(*triggerShape);
		gScene->addActor(*triggerBody);
		physx_actors.push_back({ triggerBody, physx_actors.back().actorId + 1 });
		triggerShape->release();
		ptr = triggerBody;
	}
	else {
		PxRigidDynamic* triggerBody = gPhysics->createRigidDynamic(triggerPos);
		triggerBody->attachShape(*triggerShape);
		gScene->addActor(*triggerBody);
		physx_actors.push_back({ triggerBody, physx_actors.back().actorId + 1 });
		triggerShape->release();
		ptr = triggerBody;
	}
}



void BoxTrigger::setPos(PxVec3 new_pos) {
	pos = new_pos;
}



//To disable a PxShape instance from scene query tests :
void BoxTrigger::disableShapeInSceneQueryTests()
{
	triggerShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
}

//Finally, a PxShape instance can be re - enabled in scene query tests :
void BoxTrigger::enableShapeInSceneQueryTests()
{
	triggerShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
}