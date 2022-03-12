#include "BoxTrigger.h"
#include "PxPhysicsAPI.h"
#include "physx_globals.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PxCustomEventCallback.h"


// Joints Doc:
// https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Joints.html 


using namespace physx;
using namespace snippetvehicle;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Box-shaped Trigger
BoxTrigger::BoxTrigger(bool is_static, PxVec3 pos, float width, float height, float depth) :
	origin(pos), w(width), h(height), l(depth) 
{
	// BOX = (a/2, b/2, c/2) where a,b,c are the sides of the box
	triggerShape = gPhysics->createShape(PxBoxGeometry(PxVec3(width / 2, height / 2, depth / 2)), *gMaterial);
	createTrigger(is_static);
}




// Sphere Trigger
BoxTrigger::BoxTrigger(bool is_static, physx::PxVec3 pos, float radius) :
	origin(pos), r(radius)
{
	triggerShape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
	createTrigger(is_static);
}



// Capsule Trigger
BoxTrigger::BoxTrigger(bool is_static, physx::PxVec3 pos, float radius, float height, physx::PxVec3 n) :
	origin(pos), r(radius), h(height), normal(n)
{
	triggerShape = gPhysics->createShape(PxCapsuleGeometry(radius, height / 2), *gMaterial);
	createTrigger(is_static);
}



// These shapes are rendered as boxes instead
// Plane Trigger (anything below plane will collide) CAN ONLY BE STATIC
BoxTrigger::BoxTrigger(bool is_static, PxVec3 pos, PxVec3 n) :
	origin(pos), normal(n)
{
	triggerShape = gPhysics->createShape(PxPlaneGeometry(), *gMaterial);
	createTrigger(is_static);
}



// Convex mesh Trigger
BoxTrigger::BoxTrigger(bool is_static, PxVec3 pos, PxConvexMesh* convexMesh) :
	origin(pos)
{
	triggerShape = gPhysics->createShape(PxConvexMeshGeometry(convexMesh), *gMaterial);
	createTrigger(is_static);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Creates trigger actor
// All constructors must call this function after creating a shape
void BoxTrigger::createTrigger(bool is_static) {

	// Specify that this is a trigger shape
	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);				

	// Filters
	PxFilterData triggerFilter(COLLISION_FLAG_TRIGGER, COLLISION_FLAG_TRIGGER_AGAINST, 0, 0);
	triggerShape->setSimulationFilterData(triggerFilter);

	// position of the trigger
	PxTransform triggerPos(origin);											

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



// Will add a fixed joint so that the trigger moves together with its attached actor
// Trigger must be dynamic
// Trigger origin is based on attached actor
void BoxTrigger::addJoint(PxRigidActor* actor0, const PxTransform& startTransform0) {
	PxFixedJointCreate(*gPhysics, actor0, startTransform0, ptr, PxTransform(origin));
}



// Add a primitive model for debugging purposes
void BoxTrigger::setModel() {
	primitive = new Primitive();

	switch (shape) {
		case SHAPE::BOX:
			primitive->createBox(w, h, l);
			break;

		case SHAPE::SPHERE:
			primitive->createSphere(r);
			break;

		case SHAPE::CAPSULE:
			primitive->createSphere(r); // TODO placeholder
			break;

		case SHAPE::PLANE:
			primitive->createBox(20.f, 20.f, 1.f); //TODO placeholder
			break;

		case SHAPE::CONVEX_MESH:
			primitive->createBox(w, h, l);
			break;
	}
}


// Draw model for debugging 
void BoxTrigger::draw() {
	if (primitive != NULL) {
		primitive->render();
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CURRENTLY UNUSED FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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