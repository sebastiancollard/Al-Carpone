#pragma once

#include "glm/glm.hpp"
#include "PxPhysicsAPI.h"


class BoxTrigger {
private:
	physx::PxVec3 pos = {0.f, 0.f, 0.f};
	physx::PxShape* triggerShape;

public:

	physx::PxRigidActor* ptr = NULL;

	BoxTrigger() {}
	BoxTrigger(bool is_static, float width, float height, float depth, physx::PxVec3 pos);

	void setPos(physx::PxVec3 pos);
	void disableShapeInSceneQueryTests();
	void enableShapeInSceneQueryTests();

};