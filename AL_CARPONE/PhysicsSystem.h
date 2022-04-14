#pragma once
#include <glad/glad.h>
#include "PxPhysicsAPI.h"
#include "State.h"
#include "Vehicle.h"
#include "Player.h"

using namespace physx;

//TODO
class PhysicsSystem {

private:
	State& state;
	Player& player;

public:

	PhysicsSystem(State& s, Player& p);

	void step(GLFWwindow* window);
	void simulate_vehicle(Vehicle* vehicle, float substep);
	void cleanup();

	// General Functions (//TODO some private??
	void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));
	PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices32, const PxU32 numTris, PxPhysics& physics, PxCooking& cooking);
	PxTriangleMesh* createLevelMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking, unsigned int selection);
	PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxCooking* cooking, unsigned int selection);
	PxRigidDynamic* createDynamicItem(Model item_model, const PxTransform& t, const PxVec3& velocity);
};