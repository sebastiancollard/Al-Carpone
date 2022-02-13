#pragma once
#include <iostream>
#include <queue>
#include <glm/glm.hpp>
#include "physXVehicleSettings.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include "physx_globals.h"
#include "Model.h"


enum VEHICLE_TYPE
{
	AL_CARPONE = 0,
	POLICE_CAR
};

class Vehicle {
public:

	bool vehicleInAir = true;
	unsigned int ID = -1;
	physx::PxRigidActor* actorPtr;	// Each vehicle instantiation has an actor (physx vehicle). Mostly used to query information about the car in the context of the simulation.
	physx::PxVehicleDrive4W* vehiclePtr;
	CarModel4W* car;

	std::queue<DriveMode> inputQueue;	// Input queue used to process multiple actions within a single update.

	Vehicle() {}

	Vehicle(VEHICLE_TYPE type, unsigned int ID,  PxTransform T): ID(ID) {
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

	Vehicle(VEHICLE_TYPE type, unsigned int ID, physx::PxVec3 startOffset): ID(ID) {
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

	PxTransform getStartTransform() {
		return startTransform;
	}

	void moveStartPoint(PxVec3 v) {
		startTransform.p = v;
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