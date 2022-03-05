#include "BoxTrigger.h"
#include "PxPhysicsAPI.h"
#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PxCustomEventCallback.h"


using namespace physx;
using namespace snippetvehicle;




BoxTrigger::BoxTrigger(bool is_static, float width, float height, float depth, PxVec3 t_pos) :
	w(width), h(height), l(depth) {

	std::cout << "TRIGGER BOX CREATED" << std::endl;
	std::cout << t_pos.x << ", " << t_pos.y << ", " << t_pos.z << std::endl;

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
		ptr = gPhysics->createRigidStatic(triggerPos);
	}
	else {
		ptr = gPhysics->createRigidDynamic(triggerPos);
	}


	ptr->attachShape(*triggerShape);
	gScene->addActor(*ptr);
	physx_actors.push_back({ ptr, physx_actors.back().actorId + 1 });
	triggerShape->release();

	setModel(); //render shape
}


void BoxTrigger::addJoint(PxRigidActor* actor0, const PxTransform& startTransform0) {
	PxFixedJointCreate(*gPhysics, actor0, startTransform0, ptr, PxTransform(pos));
}


void BoxTrigger::setPos(PxVec3 new_pos) {
	pos = new_pos;
	// TODO
	//void PxRigidBody::setLinearVelocity(const PxVec3 & linVel, bool autowake);
	//void PxRigidBody::setAngularVelocity(const PxVec3 & angVel, bool autowake);
}

void BoxTrigger::setModel() {
	primitive = new Primitive();
	primitive->createBox(w, h, l);
}

void BoxTrigger::draw() {
	if (primitive != NULL) {
		primitive->render();
	}
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