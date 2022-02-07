#pragma once
#include <iostream>
#include <queue>
#include "physXVehicleSettings.h"

class Vehicle {
public:

	bool vehicleInAir = true;
	PxRigidActor* actorPtr;	// Each vehicle instantiation has an actor (physx vehicle). Mostly used to query information about the car in the context of the simulation.
	PxVehicleDrive4W* vehiclePtr;

	std::queue<DriveMode> inputQueue;	// Input queue used to process multiple actions within a single update.

	Vehicle() {}

	Vehicle(unsigned int ID) {
		//Create a vehicle that will drive on the plane.

		extern VehicleDesc initVehicleDesc();
		extern void startBrakeMode();

		VehicleDesc vehicleDesc = initVehicleDesc();
		vehiclePtr = createVehicle4W(vehicleDesc, gPhysics, gCooking);
		PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
		setResetPoint(startTransform);
		vehiclePtr->getRigidDynamicActor()->setGlobalPose(startTransform);


		gScene->addActor(*vehiclePtr->getRigidDynamicActor());
		actorPtr = vehiclePtr->getRigidDynamicActor();
		physx_actors.push_back({ vehiclePtr->getRigidDynamicActor(), ID });

		//Set the vehicle to rest in first gear.
		//Set the vehicle to use auto-gears.
		vehiclePtr->setToRestState();
		vehiclePtr->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		vehiclePtr->mDriveDynData.setUseAutoGears(true);

		startBrakeMode();
	}


	glm::vec3 getLinearVelocity() {
		PxVec3 velo = vehiclePtr->getRigidDynamicActor()->getLinearVelocity();
		return glm::vec3(velo.x, velo.y, velo.z);
	}

	glm::vec3 getAngularVelocity() {
		PxVec3 velo = vehiclePtr->getRigidDynamicActor()->getAngularVelocity();
		return glm::vec3(velo.x, velo.y, velo.z);
	}
	// fetch the front-facing direction of the player vehicle
	glm::vec3 getDir() {
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
	glm::vec3 getRight() {
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

	glm::vec3 getUp() {
		glm::vec3 a = getRight();
		glm::vec3 b = getDir();

		return  glm::normalize(glm::cross(a, b));
	}

	// fetch position of the vehicle chassis
	glm::vec3 getPos() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape(PxShapeExt::getGlobalPose(*shapes[4], *actorPtr)); // index 6 here is the box, 0 through 5 are the balls, we'll have to find a better way to do this.

		return glm::vec3(shape.p.x, shape.p.y, shape.p.z);
	}

	void setResetPoint(PxTransform t) {
		startTransform = t;
	}

	void reset() {
		actorPtr->setGlobalPose(startTransform);
		vehiclePtr->getRigidDynamicActor()->setAngularVelocity(PxVec3(0, 0, 0));
		vehiclePtr->getRigidDynamicActor()->setLinearVelocity(PxVec3(0, 0, 0));
	}

protected:
	PxTransform startTransform;
};