#include <iostream>
#include <queue>

class Player {
public:
	PxRigidActor* actorPtr;				// Each player instantiation has an actor (physx vehicle). Mostly used to query information about the car in the context of the simulation.
	std::queue<DriveMode> inputQueue;	// Input queue used to process multiple actions within a single update.

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

	// fetch position of the vehicle chassis
	glm::vec3 getPos() {
		const int MAX_NUM_ACTOR_SHAPES = 128;
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		const PxU32 nbShapes = actorPtr->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actorPtr->getShapes(shapes, nbShapes);

		PxTransform shape(PxShapeExt::getGlobalPose(*shapes[6], *actorPtr)); // index 6 here is the box, 0 through 5 are the balls, we'll have to find a better way to do this.

		return glm::vec3(shape.p.x, shape.p.y, shape.p.z);
	}


	// Handle all key inputs relevant to driving
	void handleInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)				
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);		// Add accelerate forwards to the input queue if 'W' is pressed

		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)			
			inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);		// Add accelerate backwards (reverse) to the input queue if 'S' is pressed
																		// Set as an else if for now seeing as you normally can't accelerate frontwards/backwards at the same time...
		

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);		// Add left turn to the input queue if 'A' is pressed
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);	// Add right turn to the input queue if 'D' is pressed

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
			inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);			// Add handbrake to the input queue if 'spacebar' is pressed
	
		//if(glfwGetJoystickAxes(window,GLFW_JOYSTICK_1))
		//if (glfwJoystickPresent(GLFW_JOYSTICK_1));
		int JoystickPresent = glfwJoystickPresent(GLFW_JOYSTICK_1);
		if (JoystickPresent == 1) 
		{
			int axesCount;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			//std::cout << count << std::endl;

			/*
			 std::cout << "Left Stick X Axis: " << axes[0] << std::endl; // tested with PS4 controller connected via micro USB cable
			std::cout << "Left Stick Y Axis: " << axes[1] << std::endl; // tested with PS4 controller connected via micro USB cable
			std::cout << "Right Stick X Axis: " << axes[2] << std::endl; // tested with PS4 controller connected via micro USB cable
			std::cout << "Right Stick Y Axis: " << axes[3] << std::endl; // tested with PS4 controller connected via micro USB cable
			std::cout << "Left Trigger/L2: " << axes[4] << std::endl; // tested with PS4 controller connected via micro USB cable
			std::cout << "Right Trigger/R2: " << axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable
			*/

			//left joystick
			if (axes[0] < -0.25)		//left
				inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
			if (axes[0] > 0.25)			//right
				inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
			if (axes[1] < -0.5)			//forward
				inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);
			if (axes[1] > 0.5)			//backward
				inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);



			//int buttonCount;
			//const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
			//if (GLFW_PRESS == buttons[1])
			//{
			//	//std::cout << "Pressed" << std::endl;
			//}
			//else if (GLFW_RELEASE == buttons[0])
			//{
			//	//std::cout << "Released" << std::endl;
			//}

			//const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
			////std::cout << name << std::endl;

		
		}
	}	
};
