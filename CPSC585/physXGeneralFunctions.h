#pragma once

#include "physXVehicleFunctions.h"
#include "PxCustomEventCallback.h"


using namespace physx;



void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	static PxU32 dynamicCounter = 0;

	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);

	physx_actors.push_back({ dynamic, dynamicCounter++ });
}


PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices32, const PxU32 numTris, PxPhysics& physics, PxCooking& cooking) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVerts;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;

	meshDesc.triangles.count = numTris;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices32;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = cooking.cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
		return NULL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return physics.createTriangleMesh(readBuffer);
}


void createBankActors() {
	//BANK (BUILDING)
	static PxU32 counter = 0;
	PxVec3 b_pos(bank.getPos().x, bank.getPos().y, bank.getPos().z);
	
	float bank_half_width, bank_half_depth; 
	if ((bank.getDir() == orient::N) || (bank.getDir() == orient::S)) {		//if bank is facing east or west, we shoudl swap the width and depth dimensions of the bank "hitbox"
		bank_half_width = bank.getWidth() / 2.f;
		bank_half_depth = bank.getDepth() / 2.f;
	}
	else {
		bank_half_width = bank.getDepth() / 2.f;
		bank_half_depth = bank.getWidth() / 2.f;
	}
	PxVec3 dimensions(bank_half_width, bank.getHeight()/2.f, bank_half_depth);			//PxVec3 represents the half-extents (half-w, half-h, half-d).
	
	// Setting up a bank(rigidStatic) as a simple box for now
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(dimensions), *gMaterial);
	PxTransform bankPos(b_pos);																	//position of the bank
	
	PxRigidStatic* body = gPhysics->createRigidStatic(bankPos);									//Static as the buildung will not move
	PxFilterData bankFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);	//(I think) these should set this actor as an obstacle to the vehicle
	
	shape->setSimulationFilterData(bankFilter);
	body->attachShape(*shape);
	gScene->addActor(*body);
	physx_actors.push_back({ body, counter++ });
	shape->release();

	bank.bankPtr = body;
	
	//ROBBING TRIGGER
	//Setting up the capsule that will act as a trigger. This is set up in "front" of the bank (will need bank position and orientation).
	PxVec3 t_pos = b_pos;
	t_pos.y = 0.f;															//set height to 0 so the car can actually touch it
	float trigger_half_depth = 7.f;
	float trigger_half_width;
	switch (bank.getDir()) {
		case 0:		//N
			t_pos.z += (bank_half_depth + trigger_half_depth);		//trigger is further back in the z direction (higher z)
			trigger_half_width = bank.getWidth() / 2.f;
			break;
		case 1:		//E
			t_pos.x -= (bank_half_depth + trigger_half_depth);		//trigger is further "right" (x dir)
			trigger_half_width = trigger_half_depth;				//swap width and depth
			trigger_half_depth = bank.getWidth() / 2.f;
			break;
		case 2:		//S
			t_pos.z -= (bank_half_depth + trigger_half_depth);		//trigger is further forward in the z direction (smaller z)
			trigger_half_width = bank.getWidth() / 2.f;
			break;
		case 3:		//W
			t_pos.x += (bank_half_depth + trigger_half_depth);		//trigger is further "left" (x dir)
			trigger_half_width = trigger_half_depth;				//swap width and depth
			trigger_half_depth = bank.getWidth() / 2.f;
			break;
	}
	//PxShape* triggerShape = gPhysics->createShape(PxCapsuleGeometry(PxReal(5), PxReal(2.5)), *gMaterial);	//radius and half-height of capsule as parameters
	PxShape* triggerShape = gPhysics->createShape(PxBoxGeometry(PxVec3(trigger_half_width, 2.f, trigger_half_depth)), *gMaterial);
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


PxTriangleMesh* createLevelMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking, unsigned int selection)
{
	std::vector<std::string> level_physx_paths{
		"models/tuning_testlevel/tuning_testlevel_physx.obj",
		"models/racetrack/racetrack_physx.obj",
		"models/ai_testlevel/ai_testlevel_physx.obj",
		"models/city_prototype/city_prototype_physx.obj"
	};

	Model level(level_physx_paths[state.selectedLevel]);

	std::vector<PxVec3> model_positions;
	std::vector<PxU32> model_indices;

	for (int j = 0; j < level.meshes[0].vertices.size(); j++) {
	
		model_positions.push_back(PxVec3(level.meshes[0].vertices[j].Position[0], level.meshes[0].vertices[j].Position[1], level.meshes[0].vertices[j].Position[2]));
	}

	for (int j = 0; j < level.meshes[0].indices.size(); j++) {

		model_indices.push_back(level.meshes[0].indices[j]);
	}

	PxVec3* verts = model_positions.data();
	PxU32* indices = model_indices.data();

	return createTriangleMesh(verts,model_positions.size(), indices, model_indices.size() / 3, physics, cooking);
}


PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxCooking* cooking, unsigned int selection)
{
	//Add a plane to the scene.

	PxTriangleMeshGeometry levelMesh = createLevelMesh(PxVec3(0, 0, 0), *physics, *cooking, selection);
	const PxTriangleMeshGeometry* levelGeometry(&levelMesh);

	PxShape* levelShape = physics->createShape(*levelGeometry, *material);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	levelShape->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	levelShape->setSimulationFilterData(simFilterData);

	const PxTransform transform(PxVec3(0, 0, 0));
	PxRigidStatic* groundPlane = PxCreateStatic(*physics, transform, *levelShape);

	return groundPlane;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZE PHYSICS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

	gScene = gPhysics->createScene(sceneDesc);

	//Set the callback to the custom callback class (subclass of SimulationEventCallback -- this in Player.h for now)
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
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, gCooking, 0);
	gScene->addActor(*gGroundPlane);
	

	PxU32 size = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC) * sizeof(PxActor*);
	PxActor** actors = (PxActor**)malloc(size);
	gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, actors, size, 0);
	activeLevelActorPtr = actors[gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC) - 1];
	

	//Setup main player vehicle
	player = Player(0);
	//Add it to the list of active vehicles
	state.activeVehicles.push_back(&player);

	createBankActors();
}




void stepPhysics(GLFWwindow* window)
{
	float timestep = state.timeStep * state.simulationSpeed; // 1.0f / 60.0f;

	while (timestep > 0) {

		float substep = 1.0f / 60.0f;

		if (timestep < 1.0f / 60.0f) substep = timestep;

		if (state.cameraMode == CAMERA_MODE_BOUND) player.handleInput(window,state);

		updateDrivingMode(player);

		//Update the control inputs for the vehicle.dwd
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, substep, player.vehicleInAir, *player.vehiclePtr);
		
		for (Vehicle* v : state.activeVehicles) {
			//Raycasts.
			PxVehicleWheels* vehicles[1] = { v->vehiclePtr };
			PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
			const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
			PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);
		

			//Vehicle update.
			const PxVec3 grav = gScene->getGravity();
			PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
			PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, player.vehiclePtr->mWheelsSimData.getNbWheels()} };
			PxVehicleUpdates(substep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

			//Work out if the vehicle is in the air.
			v->vehicleInAir = v->vehiclePtr->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		}

		

		//Scene update.
		gScene->simulate(substep);
		gScene->fetchResults(true);

		timestep -= (1.0f / 60.0f);
	}
}
	

void cleanupPhysics()
{
	player.vehiclePtr->getRigidDynamicActor()->release();
	player.vehiclePtr->free();
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
