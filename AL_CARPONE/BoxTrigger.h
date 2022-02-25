#pragma once

#include "glm/glm.hpp"
#include "PxPhysicsAPI.h"
#include "Model.h"


class BoxTrigger {
private:
	physx::PxVec3 pos = {0.f, 0.f, 0.f};
	physx::PxShape* triggerShape = NULL;
	Model* model = NULL;

public:

	physx::PxRigidActor* ptr = NULL;

	BoxTrigger() {}
	BoxTrigger(bool is_static, float width, float height, float depth, physx::PxVec3 pos);

	void setPos(physx::PxVec3 pos);
	void setModel();
	void disableShapeInSceneQueryTests();
	void enableShapeInSceneQueryTests();

};