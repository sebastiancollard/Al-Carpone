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


void Vehicle::setStart(PxVec3 pos, PxVec3 dir) {

	startDirection = glm::normalize(glm::vec3(dir.x,dir.y,dir.z));

	PxTransform T;
	T.p = pos;
	dir.normalize();

	glm::vec3 glmDir(dir.x, dir.y, dir.z);

	PxQuat selection;
	glm::vec3 selectionComparer;

	PxQuat NORTH(0, 1, 0, 0);
	glm::vec3 glmNORTH(0, 0, -1);

	selection = NORTH;
	selectionComparer = glmNORTH;

	PxQuat EAST(0, sqrt(0.5), 0, sqrt(0.5));
	glm::vec3 glmEAST(1, 0, 0);

	if (glm::length(glmEAST - glmDir) < glm::length(selectionComparer - glmDir)) {
		selection = EAST;
		selectionComparer = glmEAST;
	}

	PxQuat SOUTH(0, 0, 0, -1);
	glm::vec3 glmSOUTH(0, 0, 1);

	if (glm::length(glmSOUTH - glmDir) < glm::length(selectionComparer - glmDir)) {
		selection = SOUTH;
		selectionComparer = glmSOUTH;
	}

	PxQuat WEST(0, -sqrt(0.5), 0, -sqrt(0.5));
	glm::vec3 glmWEST(-1, 0, 0);

	if (glm::length(glmWEST - glmDir) < glm::length(selectionComparer - glmDir)) {
		selection = WEST;
		selectionComparer = glmWEST;
	}

	T.q = selection;

	startTransform = T;
}


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

void Vehicle::updateSpeed(double newSpeed) {
	speed = newSpeed;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Fetch Directions
///////////////////////////////////////////////////////////////////////////////////////////////////////

PxMat44 Vehicle::getTransform() const {
	PxMat44 mat = this->vehiclePtr->getRigidDynamicActor()->getGlobalPose();
	return mat.getTranspose();
}

// fetch the front-facing direction of the player vehicle
glm::vec3 Vehicle::getDir() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][2], transformMat[1][2], transformMat[2][2]));
}


// fetch the vector pointing to the right of the vehicle
glm::vec3 Vehicle::getRight() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][0], transformMat[1][0], transformMat[2][0]));
}


glm::vec3 Vehicle::getUp() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][1], transformMat[1][1], transformMat[2][1]));
}


// fetch position of the vehicle chassis
glm::vec3 Vehicle::getPos() {
	PxVec3 pxPos = actorPtr->getGlobalPose().p;
	return glm::vec3(pxPos.x, pxPos.y, pxPos.z);
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