#pragma once
#include "PxCustomEventCallback.h"

void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	static PxU32 dynamicCounter = 0;

	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);

	physx_actors.push_back({ dynamic, dynamicCounter++ });
}

void createBankActors() {
	
	static PxU32 counter = 0;
	PxVec3 b_pos(bank.getPos().x, bank.height/2.f, bank.getPos().z);
	PxVec3 dimensions(bank.width/2.f, bank.height/2.f, bank.depth/2.f);
	
	// Setting up a bank(rigidStatic) as a simple box for now
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(dimensions), *gMaterial);			//PxVec3 represents the half-extents (w, h, d). Put in arbitrary values for now
	PxTransform bankPos(b_pos);																	//position of the bank
	
	PxRigidStatic* body = gPhysics->createRigidStatic(bankPos);									//Static as the buildung will not move
	PxFilterData bankFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);	//(I think) these should set this actor as an obstacle to the vehicle
	
	shape->setSimulationFilterData(bankFilter);
	body->attachShape(*shape);
	gScene->addActor(*body);
	physx_actors.push_back({ body, counter++ });
	shape->release();

	bank.bankPtr = body;
	
	//Setting up the capsule that will act as a trigger. This is set up in "front" of the bank (will need bank position and orientation).
	PxVec3 t_pos = b_pos;
	switch (bank.getDir()) {
		case 1:		//N
			t_pos.y -= ((bank.depth / 2.f) + bank.width/2.f);		//trigger is further back in the y direction
			break;
		case 2:		//E
			t_pos.x += ((bank.width / 2.f) + bank.width / 2.f);		//trigger is further "right"
			break;
		case 3:		//S
			t_pos.y += ((bank.depth / 2.f) + bank.width / 2.f);		//trigger is further forward in the y direction
			break;
		case 4:		//W
			t_pos.x += ((bank.width / 2.f) + bank.width / 2.f);		//trigger is further "left"
			break;
	}

	PxShape* triggerShape = gPhysics->createShape(PxCapsuleGeometry(PxReal(5), PxReal(10)), *gMaterial);	//radius and half-height of capsule as parameters
	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);		//This is a trigger shape.

	PxTransform triggerPos(t_pos);											//position of the trigger.
	
	PxRigidStatic* triggerBody = gPhysics->createRigidStatic(triggerPos);
	PxFilterData triggerFilter(COLLISION_FLAG_BANK_TRIGGER, COLLISION_FLAG_BANK_TRIGGER_AGAINST, 0, 0);

	triggerShape->setSimulationFilterData(triggerFilter);
	triggerBody->attachShape(*triggerShape);
	gScene->addActor(*triggerBody);
	physx_actors.push_back({ triggerBody, counter++ });
	triggerShape->release();

	bank.triggerPtr = triggerBody;
}

void initPhysics()
{
	unsigned int ID = 0;
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	//PxCustomEventCallback callback;
	//sceneDesc.simulationEventCallback = &callback;


	gScene = gPhysics->createScene(sceneDesc);

	//Set the callback to the custom callback class (subclass of SimulationEventCallback -- this in Player.h for now)
	PxCustomEventCallback callback;
	gScene->setSimulationEventCallback(&callback);
	

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);


	gScene->addActor(*gVehicle4W->getRigidDynamicActor());
	player.actorPtr = gVehicle4W->getRigidDynamicActor();
	physx_actors.push_back({ gVehicle4W->getRigidDynamicActor(), ID++ });

	createBankActors();

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	startBrakeMode();
}



void stepPhysics(GLFWwindow* window)
{
	const float timestep = 1.0f / 60.0f;

	if(state.cameraMode == CAMERA_MODE_BOUND || state.cameraMode == CAMERA_MODE_BOUND_FREELOOK) player.handleInput(window);

	updateDrivingMode();

	//Update the control inputs for the vehicle.dwd
	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	//Scene update.
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}

void cleanupPhysics()
{
	gVehicle4W->getRigidDynamicActor()->release();
	gVehicle4W->free();
	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(gAllocator);
	PX_RELEASE(gFrictionPairs);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetVehicle4W done.\n");
}
