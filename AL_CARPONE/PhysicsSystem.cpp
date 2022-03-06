#include "PhysicsSystem.h"

#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "physXVehicleFunctions.h"
#include "PxCustomEventCallback.h"
#include "physx_globals.h"
#include "util.h"

#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL; }


using namespace physx;
using namespace snippetvehicle;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZE PHYSICS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


PhysicsSystem::PhysicsSystem(State& s, Player& p) : state(s), player(p)
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

	//Set the callback to the custom callback class (subclass of SimulationEventCallback)
	gScene->setSimulationEventCallback(new PxCustomEventCallback(state, player));


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

	ai_paths = {
		//BANK
		{
			glm::vec3(559.948730, 31.299992, -360.090973),
			glm::vec3(419.948730, 31.299992, -360.090973),
			glm::vec3(419.948730, 31.299992, -500.090973),
			glm::vec3(559.948730, 31.299992, -500.090973)
		},
		//SUBURBS
		{
			glm::vec3(420.010925, 31.455765, -280.130432),
			glm::vec3(740.010925, 31.455765, -280.130432),
			glm::vec3(739.948730, 21.455765, -60.534622),
			glm::vec3(419.948730, 21.455765, -60.534622)
		},
		//DOWNTOWN1
		{
			glm::vec3(100.000031, 0.299998, -220.079498),
			glm::vec3(220.000031, 0.299998, -220.079498),
			glm::vec3(220.000031, 0.299998, -120.079498),
			glm::vec3(100.000031, 0.299998, -120.079498)
		},
		//DOWNTOWN2
		{
			glm::vec3(-99.999969, 0.299998, -220.079498),
			glm::vec3(0.000031, 0.299998, -220.079498),
			glm::vec3(0.000031, 0.299998, -60.079498),
			glm::vec3(-99.999969, 0.299998, -60.079498)
		}
	};
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STEP PHYSICS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void PhysicsSystem::step(GLFWwindow* window)
{
	float timestep = state.timeStep * state.simulationSpeed; // 1.0f / 60.0f;

	while (timestep > 0) {

		float substep = 1.0f / 60.0f;

		if (timestep < 1.0f / 60.0f) substep = timestep;

		for (int i = 0; i < state.activeVehicles.size(); i++) {

			//Update the control inputs for the vehicle.dwd
			if (state.activeVehicles[i] == &player) {
				if (state.cameraMode == CAMERA_MODE_BOUND) player.handleInput(window, state);
			}
			else {
				// if patrolling
				if (((PoliceCar*)state.activeVehicles[i])->state == AISTATE::PATROL) {
					// if distance between cop car and its target is less than the threshold, switch to next target location
					if (glm::distance(state.activeVehicles[i]->getPos(), ai_paths[i - 1][state.activeVehicles[i]->targetIndex]) < 10.f) {
						// set target index to the next location. if we're at the end of the location list, target the first location again
						state.activeVehicles[i]->targetIndex < ai_paths[i - 1].size() - 1 ? state.activeVehicles[i]->targetIndex++ : state.activeVehicles[i]->targetIndex = 0;
						//std::cout << "new target: " << ai_paths[i - 1][state.activeVehicles[i]->targetIndex].x << ", " << ai_paths[i - 1][state.activeVehicles[i]->targetIndex].y << ", " << ai_paths[i - 1][state.activeVehicles[i]->targetIndex].z << std::endl;
					}
					// determine AI inputs
					state.activeVehicles[i]->handle(window, ai_paths[i - 1][state.activeVehicles[i]->targetIndex]);
				}
				else if (((PoliceCar*)state.activeVehicles[i])->state == AISTATE::CHASE) {
					state.activeVehicles[i]->handle(window, player.getPos());
				}
				// else if idle...
			}
			updateDrivingMode(*state.activeVehicles[i]);
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, substep, state.activeVehicles[i]->vehicleInAir, *state.activeVehicles[i]->vehiclePtr);


			//Raycasts.
			PxVehicleWheels* vehicles[1] = { state.activeVehicles[i]->vehiclePtr };
			PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
			const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
			PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);


			//Vehicle update.
			const PxVec3 grav = gScene->getGravity();
			PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
			PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, player.vehiclePtr->mWheelsSimData.getNbWheels()} };
			PxVehicleUpdates(substep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

			//Work out if the vehicle is in the air.
			state.activeVehicles[i]->vehicleInAir = state.activeVehicles[i]->vehiclePtr->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		}

		//Scene update.
		gScene->simulate(substep);
		gScene->fetchResults(true);

		timestep -= (1.0f / 60.0f);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup PHYSICS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void PhysicsSystem::cleanup()
{
	// Free Vehicle Pointers
	for (Vehicle* v : state.activeVehicles) {
		v->vehiclePtr->getRigidDynamicActor()->release();
		v->vehiclePtr->free();
	}

	// TODO release trigger actors?

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
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GENERAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void PhysicsSystem::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	static PxU32 dynamicCounter = 0;

	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);

	physx_actors.push_back({ dynamic, dynamicCounter++ });
}


PxTriangleMesh* PhysicsSystem::createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices32, const PxU32 numTris, PxPhysics& physics, PxCooking& cooking) {
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





PxTriangleMesh* PhysicsSystem::createLevelMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking, unsigned int selection)
{
	std::vector<std::string> level_physx_paths{
		"models/map/map_physx.obj",
		"models/tuning_testlevel/tuning_testlevel_physx.obj",
		"models/racetrack/racetrack_physx.obj"
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

	return createTriangleMesh(verts, model_positions.size(), indices, model_indices.size() / 3, physics, cooking);
}


PxRigidStatic* PhysicsSystem::createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxCooking* cooking, unsigned int selection)
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