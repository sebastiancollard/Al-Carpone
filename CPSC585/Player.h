#pragma once

#include <iostream>
#include <queue>

class Player {
public:
	PxRigidActor* actorPtr;				// Each player instantiation has an actor (physx vehicle). Mostly used to query information about the car in the context of the simulation.
	std::queue<DriveMode> inputQueue;	// Input queue used to process multiple actions within a single update.

	glm::vec3 getLinearVelocity() {
		extern PxVehicleDrive4W* gVehicle4W;
		PxVec3 velo = gVehicle4W->getRigidDynamicActor()->getLinearVelocity();
		return glm::vec3(velo.x, velo.y, velo.z);
	}

	glm::vec3 getAngularVelocity() {
		extern PxVehicleDrive4W* gVehicle4W;
		PxVec3 velo = gVehicle4W->getRigidDynamicActor()->getAngularVelocity();
		return glm::vec3(velo.x, velo.y, velo.z);
	}

	// fetch the front-facing direction of the player vehicle
	glm::vec3 getDir() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape;

		// fetch position of two wheels, both along the same side of the car and compute a direction from that
		shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[0], *actorPtr));
		glm::vec3 a(shape.p.x, shape.p.y, shape.p.z);

		shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[2], *actorPtr));
		glm::vec3 b(shape.p.x, shape.p.y, shape.p.z);

		return glm::normalize(a - b);
	}

	// fetch the vector pointing to the right of the vehicle
	glm::vec3 getRight() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape;

		// fetch position of the front two wheels of the car and find right by that
		shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[0], *actorPtr));
		glm::vec3 a(shape.p.x, shape.p.y, shape.p.z);

		shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[1], *actorPtr));
		glm::vec3 b(shape.p.x, shape.p.y, shape.p.z);

		return glm::normalize(a - b);
	}

	glm::vec3 getUp() {
		glm::vec3 a = getRight();
		glm::vec3 b = getDir();

		return  glm::normalize(glm::cross(a, b));
	}

	// fetch position of the vehicle chassis
	glm::vec3 getPos() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape(PxShapeExt::getGlobalPose(*shapes[4], *actorPtr)); // index 6 here is the box, 0 through 5 are the balls, we'll have to find a better way to do this.

		return glm::vec3(shape.p.x, shape.p.y, shape.p.z);
	}


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed
			if (gIsVehicleInAir) {
				
			}
		}
			
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
		}
																		// Set as an else if for now seeing as you normally can't accelerate frontwards/backwards at the same time...
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		// Add left turn to the input queue if 'A' is pressed
		}
			
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	// Add right turn to the input queue if 'D' is pressed
		}
			
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);			// Add handbrake to the input queue if 'spacebar' is pressed
		}
			

	}	

	void setResetPoint(PxTransform t) {
		startTransform = t;
	}

	void reset() {
		actorPtr->setGlobalPose(startTransform);
	}

private:
	PxTransform startTransform;
};
