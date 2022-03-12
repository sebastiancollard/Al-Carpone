#pragma once

#include "glm/glm.hpp"
#include "PxPhysicsAPI.h"
#include "Primitive.h"


// https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Geometry.html
enum SHAPE {
	BOX = 0,
	SPHERE,
	CAPSULE,
	PLANE,
	CONVEX_MESH
};


/*
 * General Trigger Class
 * Event Handler is in "PxCustomEventCallback.h"
*/


class BoxTrigger {
private:

	SHAPE shape = SHAPE::BOX;

	float r, w, h, l = 3.f;
	physx::PxVec3 origin = { 0.f, 0.f, 0.f };
	physx::PxVec3 normal = { 0.f, 1.f, 0.f };
	physx::PxShape* triggerShape = NULL;
	Primitive* primitive = NULL;

	void createTrigger(bool is_static);

public:

	physx::PxRigidActor* ptr = NULL;

	BoxTrigger(bool is_static, physx::PxVec3 pos, float width, float height, float depth);		// Box Trigger
	BoxTrigger(bool is_static, physx::PxVec3 pos, float radius);								// Sphere Trigger
	BoxTrigger(bool is_static, physx::PxVec3 pos, float radius, float height, physx::PxVec3 n);	// Capsule Trigger
	BoxTrigger(bool is_static, physx::PxVec3 pos, physx::PxVec3 n);								// Plane Trigger (anything below plane will collide) CAN ONLY BE STATIC

	// Theis is rendered as a box instead
	BoxTrigger(bool is_static, physx::PxVec3 pos, physx::PxConvexMesh* convexMesh);				// Convex mesh Trigger (not sure if we should a Model in this case instead of a Primitive)


	void addJoint(physx::PxRigidActor* actor0, const physx::PxTransform & startTransform0);
	void setModel();
	void draw();
	void disableShapeInSceneQueryTests();
	void enableShapeInSceneQueryTests();
};