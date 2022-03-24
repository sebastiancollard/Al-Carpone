#pragma once
#include <glad/glad.h>
#include "PxPhysicsAPI.h"
#include "State.h"
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
	void stepPlayer(GLFWwindow* window);
	void stepAI();
	void cleanup();

	// General Functions (//TODO some private??
	void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));
	PxRigidDynamic* createDynamicItem(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);
	PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices32, const PxU32 numTris, PxPhysics& physics, PxCooking& cooking);
	PxTriangleMesh* createLevelMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking, unsigned int);
	PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics, PxCooking* cooking, unsigned int selection);
};