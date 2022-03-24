#include "PhysicsSystem.h"

#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "physXVehicleFunctions.h"
#include "PxCustomEventCallback.h"
#include "physx_globals.h"
#include "util.h"
#include "State.h"
#include "PoliceCar.h"

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
	PxInitExtensions(*gPhysics, gPvd);

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

	//load level in main

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, gCooking, 0);
	gScene->addActor(*gGroundPlane);


	PxU32 size = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC) * sizeof(PxActor*);
	PxActor** actors = (PxActor**)malloc(size);
	gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, actors, size, 0);
	activeLevelActorPtr = actors[gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC) - 1];
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STEP PHYSICS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PhysicsSystem::step(GLFWwindow* window)
{
	float timestep = state.timeStep * state.simulationSpeed; 

	float substepSize = 1 / 10.f;

	std::vector<Vehicle*> activevehicles;

	for (PoliceCar* p : state.activePoliceVehicles) {
		p->update(player.getPos(), state.timeStep);
		activevehicles.push_back(p);
	}
	if(state.cameraMode == CAMERA_MODE_BOUND) player.handleInput(window, state);
	activevehicles.push_back(&player);

	while (timestep > 0) {

		float substep = substepSize;

		if (timestep < substepSize) substep = timestep;

		for (int i = 0; i < activevehicles.size(); i++) {

			updateDrivingMode(*activevehicles[i]);
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, substep, activevehicles[i]->vehicleInAir, *activevehicles[i]->vehiclePtr);


			//Raycasts.
			PxVehicleWheels* vehicles[1] = { activevehicles[i]->vehiclePtr };
			PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
			const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
			PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);


			//Vehicle update.
			const PxVec3 grav = gScene->getGravity();
			PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
			PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, player.vehiclePtr->mWheelsSimData.getNbWheels()} };
			PxVehicleUpdates(substep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

			//Work out if the vehicle is in the air.
			activevehicles[i]->vehicleInAir = activevehicles[i]->vehiclePtr->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
		}

		//Scene update.
		gScene->simulate(substep);
		gScene->fetchResults(true);

		timestep -= substepSize;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup PHYSICS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void PhysicsSystem::cleanup()
{
	std::vector<Vehicle*> activevehicles;

	for (PoliceCar* p : state.activePoliceVehicles) {
		activevehicles.push_back(p);
	}
	activevehicles.push_back(&player);

	// Free Vehicle Pointers
	for (Vehicle* v : activevehicles) {
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

PxRigidDynamic* PhysicsSystem::createDynamicItem(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	static PxU32 dynamicCounter = 0;

	PxShape* shape = gPhysics->createShape(geometry, *gMaterial);
	PxFilterData filter(COLLISION_FLAG_ITEM, COLLISION_FLAG_ITEM_AGAINST, 0, 0);
	shape->setSimulationFilterData(filter);

	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, *shape, 10.0f);

	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);

	physx_actors.push_back({ dynamic, dynamicCounter++ });
	return dynamic;
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
	std::string level_physx_path = "models/map/map_physx.obj";
	std::string garage_door_path = "models/map/garage_door.obj";
	
	std::string path = level_physx_path;
	if (selection == 1) path = garage_door_path;

	Model level(path);

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