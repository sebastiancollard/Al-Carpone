#include <iostream>
#include <vector>
#include <queue>

class Player {
public:

	PxRigidActor* actorPtr;
	glm::vec3 dir;

	std::queue<DriveMode> inputQueue;

	//DriveMode driveMode;
	//DriveMode previousDriveMode;
	//bool driveModeChanged = false;

	glm::vec3 getDir() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape;

		shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[0], *actorPtr));
		glm::vec3 a(shape.p.x, shape.p.y, shape.p.z);

		shape = PxTransform(PxShapeExt::getGlobalPose(*shapes[2], *actorPtr));
		glm::vec3 b(shape.p.x, shape.p.y, shape.p.z);

		return glm::normalize(a - b);
	}

	glm::vec3 getPos() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape(PxShapeExt::getGlobalPose(*shapes[6], *actorPtr)); //index 6 here is the box, 0 through 5 are the balls, wel have to find a better way to do this.

		return glm::vec3(shape.p.x, shape.p.y, shape.p.z);
	}


	void handleInput(GLFWwindow* window)
	{
		// Handles key inputs
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);

		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);
		

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
		
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
			inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);
		

		//if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		//{
		//
		//}
		//if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		//{
		//
		//}
		//else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		//{
		//
		//}
		
	}
};