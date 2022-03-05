#include "Vehicle.h"
#include "State.h"



using namespace physx;


///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////////////////////////////



Vehicle::Vehicle(VEHICLE_TYPE type, unsigned int ID, PxTransform T) : ID(ID) {
	//Create a vehicle that will drive on the plane.

	extern snippetvehicle::VehicleDesc initVehicleDesc(VEHICLE_TYPE);
	extern void startBrakeMode();

	snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc(type);
	vehiclePtr = createVehicle4W(vehicleDesc, gPhysics, gCooking, type);
	physx::PxTransform startTransform = T;
	setResetPoint(startTransform);
	vehiclePtr->getRigidDynamicActor()->setGlobalPose(startTransform);


	gScene->addActor(*vehiclePtr->getRigidDynamicActor());
	actorPtr = vehiclePtr->getRigidDynamicActor();
	if (physx_actors.size() == 0) {
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), 0 });
	}
	else {
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), physx_actors.back().actorId + 1 });
	}

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	vehiclePtr->setToRestState();
	vehiclePtr->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
	vehiclePtr->mDriveDynData.setUseAutoGears(true);
}


Vehicle::Vehicle(VEHICLE_TYPE type, unsigned int ID, physx::PxVec3 startOffset) : ID(ID) {
	//Create a vehicle that will drive on the plane.

	extern snippetvehicle::VehicleDesc initVehicleDesc(VEHICLE_TYPE);
	extern void startBrakeMode();

	snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc(type);
	vehiclePtr = createVehicle4W(vehicleDesc, gPhysics, gCooking, type);
	physx::PxTransform startTransform(physx::PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0) + startOffset, physx::PxQuat(PxIdentity));
	setResetPoint(startTransform);
	vehiclePtr->getRigidDynamicActor()->setGlobalPose(startTransform);


	gScene->addActor(*vehiclePtr->getRigidDynamicActor());
	actorPtr = vehiclePtr->getRigidDynamicActor();
	if (physx_actors.size() == 0) {
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), 0 });
	}
	else {
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), physx_actors.back().actorId + 1 });
	}


	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	vehiclePtr->setToRestState();
	vehiclePtr->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	vehiclePtr->mDriveDynData.setUseAutoGears(true);

}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// TRANSFORMS
///////////////////////////////////////////////////////////////////////////////////////////////////////


PxTransform Vehicle::getStartTransform() {
	return startTransform;
}


void Vehicle::moveStartPoint(PxVec3 v) {
	startTransform.p = v;
}


glm::vec3 Vehicle::getLinearVelocity() {
	PxVec3 velo = vehiclePtr->getRigidDynamicActor()->getLinearVelocity();
	return glm::vec3(velo.x, velo.y, velo.z);
}


glm::vec3 Vehicle::getAngularVelocity() {
	PxVec3 velo = vehiclePtr->getRigidDynamicActor()->getAngularVelocity();
	return glm::vec3(velo.x, velo.y, velo.z);
}

float Vehicle::getForwardVelocity() {
	return glm::dot(getLinearVelocity(), getDir());
}

float Vehicle::getForwardAcceleration() {
	return glm::dot(acceleration, getDir());
}

float Vehicle::getForwardJerk() {
	return glm::dot(jerk, getDir());
}

void Vehicle::updatePhysicsVariables(double dt) {

	if (dt == 0) return;

	glm::vec3 newVelocity = getLinearVelocity(); 
	glm::vec3 newAcceleration = (newVelocity - velocity) / (float)dt; // a = dv/dt
	glm::vec3 newJerk = (newAcceleration - acceleration) / (float)dt; // j = da/dt = ddv/ddt

	jerk = newJerk;
	acceleration = newAcceleration;
	velocity = newVelocity;

	//printf("dt[%.4f]	VELOCITY[%.5f]	ACCELERATION[%.5f]	JERK[%.5f]\n",(float)dt, glm::length(velocity),glm::length(acceleration),glm::length(jerk));	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Fetch Directions
///////////////////////////////////////////////////////////////////////////////////////////////////////



// fetch the front-facing direction of the player vehicle
glm::vec3 Vehicle::getDir() {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	const PxU32 nbShapes = actorPtr->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	actorPtr->getShapes(shapes, nbShapes);

	PxTransform shape;

	// fetch position of two wheels, both along the same side of the car and compute a direction from that
	shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[0], *actorPtr));
	glm::vec3 a(shape.p.x, shape.p.y, shape.p.z);

	shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[2], *actorPtr));
	glm::vec3 b(shape.p.x, shape.p.y, shape.p.z);

	return glm::normalize(a - b);
}


// fetch the vector pointing to the right of the vehicle
glm::vec3 Vehicle::getRight() {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	const PxU32 nbShapes = actorPtr->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	actorPtr->getShapes(shapes, nbShapes);

	PxTransform shape;

	// fetch position of the front two wheels of the car and find right by that
	shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[0], *actorPtr));
	glm::vec3 a(shape.p.x, shape.p.y, shape.p.z);

	shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[1], *actorPtr));
	glm::vec3 b(shape.p.x, shape.p.y, shape.p.z);

	return glm::normalize(a - b);
}


glm::vec3 Vehicle::getUp() {
	glm::vec3 a = getRight();
	glm::vec3 b = getDir();

	return  glm::normalize(glm::cross(a, b));
}


// fetch position of the vehicle chassis
glm::vec3 Vehicle::getPos() {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	const PxU32 nbShapes = actorPtr->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	actorPtr->getShapes(shapes, nbShapes);

	PxTransform shape(PxShapeExt::getGlobalPose(*shapes[4], *actorPtr)); // index 6 here is the box, 0 through 5 are the balls, we'll have to find a better way to do this.

	return glm::vec3(shape.p.x, shape.p.y, shape.p.z);
}


bool Vehicle::isMoving() {
	return glm::length(getLinearVelocity()) > 0.0f;
}

bool Vehicle::isChangingGears() {
	return vehicleChangingGears;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// RESETS
///////////////////////////////////////////////////////////////////////////////////////////////////////


void Vehicle::setResetPoint(PxTransform t) {
	startTransform = t;
}


void Vehicle::reset() {
	actorPtr->setGlobalPose(startTransform);
	vehiclePtr->getRigidDynamicActor()->setAngularVelocity(PxVec3(0, 0, 0));
	vehiclePtr->getRigidDynamicActor()->setLinearVelocity(PxVec3(0, 0, 0));
}