#pragma once

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

PxTriangleMesh* createLevelMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
{
	Model level(ACTIVE_LEVEL_PHYSX_MODEL_PATH);

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
PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxCooking* cooking)
{
	//Add a plane to the scene.

	PxTriangleMeshGeometry levelMesh = createLevelMesh(PxVec3(0, 0, 0), *physics, *cooking);
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
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, gCooking);
	gScene->addActor(*gGroundPlane);

	//Setup main player vehicle
	player = Player(ID);
	//Add it to the list of active vehicles
	activeVehicles.push_back(&player);
}



void stepPhysics(GLFWwindow* window)
{

	float timestep = state.timeStep * state.simulationSpeed; // 1.0f / 60.0f;

	while (timestep > 0) {

		float substep = 1.0f / 60.0f;

		if (timestep < 1.0f / 60.0f) substep = timestep;

		if (state.cameraMode == CAMERA_MODE_BOUND) player.handleInput(window);

		updateDrivingMode();

		//Update the control inputs for the vehicle.dwd
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, substep, player.vehicleInAir, *player.vehiclePtr);

		//Raycasts.
		PxVehicleWheels* vehicles[1] = { player.vehiclePtr };
		PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const PxVec3 grav = gScene->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, player.vehiclePtr->mWheelsSimData.getNbWheels()} };
		PxVehicleUpdates(substep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

		//Work out if the vehicle is in the air.
		player.vehicleInAir = player.vehiclePtr->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

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
