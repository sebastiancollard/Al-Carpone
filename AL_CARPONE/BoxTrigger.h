#pragma once

#include "glm/glm.hpp"
#include "PxPhysicsAPI.h"
#include "Primitive.h"


class BoxTrigger {
private:

	float w, h, l;
	physx::PxVec3 pos = {0.f, 0.f, 0.f};
	physx::PxShape* triggerShape = NULL;
	Primitive* primitive = NULL;

public:

	physx::PxRigidActor* ptr = NULL;

	BoxTrigger() {}
	BoxTrigger(bool is_static, float width, float height, float depth, physx::PxVec3 pos);

	void setPos(physx::PxVec3 pos);
	void addJoint(physx::PxRigidActor* actor0, const physx::PxTransform & startTransform0);
	void setModel();
	void draw();
	void disableShapeInSceneQueryTests();
	void enableShapeInSceneQueryTests();
};