//Initializes the physx system and all global variables and calls other includes.

#include"init.h"

#define CAR_CHASSIS_PATH "models/car/car_chassis.obj"
#define CAR_LWHEEL_PATH "models/car/car_Lwheel.obj"
#define CAR_RWHEEL_PATH "models/car/car_Rwheel.obj"

#define TEST_LEVEL_PATH "models/testlevel/ai_testlevel.obj"

#define NEAR_CLIPPING_PLANE 0.01f
#define FAR_CLIPPING_PLANE 1000.f

enum PART
{
	CHASSIS = 0,
	FLWHEEL,
	FRWHEEL,
	BLWHEEL,
	BRWHEEL
};

class CarModel4W {
public:
	CarModel4W(Model Chassis, Model LWheel, Model RWheel) :
		Chassis(Chassis), LWheel(LWheel), RWheel(RWheel){
	}
	
	void Draw(unsigned int part, Shader& shader, glm::mat4 model_physx) {

		glm::vec3 commonVerticalOffset = player.getUp() * -0.1f;

		glm::vec3 chassisVerticalOffset = player.getUp() * -1.75f;
		glm::vec3 wheelVerticalOffset = player.getUp() * 0.f;

		glm::vec3 fWheelForwardOffset = player.getDir() * 0.070f;
		glm::vec3 bWheelForwardOffset = player.getDir() * 0.55f;

		
		glm::vec3 rWheelInwardOffset = player.getRight()  * -0.2f;
		glm::vec3 lWheelInwardOffset = -rWheelInwardOffset;

		glm::mat4 model(1.0f);

		if (part == CHASSIS) {
			model = model * model_physx;
			shader.setMat4("model", model);
			Chassis.Draw(shader);
			return;
		}
		else if (part == FLWHEEL) {
			model = model * model_physx;
			shader.setMat4("model", model);
			LWheel.Draw(shader);
			return;
		}
		else if (part == FRWHEEL) {
			model = model * model_physx;
			shader.setMat4("model", model);
			RWheel.Draw(shader);
			return;
		}
		else if (part == BLWHEEL) {
			model = model * model_physx;
			shader.setMat4("model", model);
			LWheel.Draw(shader);
			return;
		}
		else if (part == BRWHEEL) {
			model = model * model_physx;
			shader.setMat4("model", model);
			RWheel.Draw(shader);
			return;
		}
	}

private:
	Model Chassis;
	Model LWheel;
	Model RWheel;
};


int main()
{

	//Set up the GLFWwindow
	GLFWwindow* window;
	{
		// Initialize GLFW
		glfwInit();

		// Tell GLFW what version of OpenGL we are using 
		// In this case we are using OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// Tell GLFW we are using the CORE profile
		// So that means we only have the modern functions
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create a GLFWwindow object of 800 by 800 pixels
		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Al Carpone", NULL, NULL);
		// Error check if the window fails to create
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		// Introduce the window into the current context
		glfwMakeContextCurrent(window);

		//Load GLAD so it configures OpenGL
		gladLoadGL();
		// Specify the viewport of OpenGL in the Window
		// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vs", "default.fs");

	//Set up physx with vehicle snippet
	//Make sure this is called after the shader program is generated
	initPhysics();

	//Create base meshes
	std::vector<Model> models;

	Model car_chassis(CAR_CHASSIS_PATH);
	Model car_lwheel(CAR_LWHEEL_PATH);
	Model car_rwheel(CAR_RWHEEL_PATH);

	CarModel4W car(car_chassis, car_lwheel, car_rwheel);

	Model groundPlane(TEST_LEVEL_PATH);
	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);



	// Creates camera pointer
	Camera* activeCamera;
	// Camrea can be one of these at a given time
	BoundCamera boundCamera; // Locked in a sphere around the car
	FreeCamera freeCamera; // Move and look freely anywhere (for debugging)
	// Init to bound camera
	activeCamera = &boundCamera;

	// Main while loop
	while (!glfwWindowShouldClose(window) && !state.terminateProgram)
	{
		//Update the time and fps counter.
		state.updateTime();
		if (state.timeStep >= 1.0f / 30.0f) {
			updateTitle(window);
			state.prevTime = state.currTime;
			state.frameCount = 0;
		}

		// Take care of all GLFW events
		glfwPollEvents();
		//Simulate physics through the timestep
		stepPhysics(window);

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use

		shaderProgram.use();

		// Handle ball input
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			createDynamic(PxTransform(
				PxVec3(activeCamera->pos.x, activeCamera->pos.y, activeCamera->pos.z)),
				PxSphereGeometry(4),
				PxVec3(activeCamera->dir.x , activeCamera->dir.y, activeCamera->dir.z) * 175.0f
			);

		//Check for special inputs (currently only camera mode change)
		checkSpecialInputs(window);

		if (state.cameraMode == CAMERA_MODE_BOUND) activeCamera = &boundCamera;
		else if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) activeCamera = &freeCamera;

		activeCamera->handleInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		shaderProgram.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(activeCamera->zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);
		glm::mat4 view = activeCamera->GetViewMatrix();

		printMat4(projection* view);

		// send them to shader
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		shaderProgram.setMat4("model", model);
		groundPlane.Draw(shaderProgram);

		// Render dynamic physx shapes
		
		
		{
			const int MAX_NUM_ACTOR_SHAPES = 128;
			PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
			
			// Loop over each actor in the scene
			for (PxU32 i = 0; i < static_cast<PxU32>(physx_actors.size()); i++)
			{
				// Fetch the number of shapes that make up the actor
				const PxU32 nbShapes = physx_actors[i].actorPtr->getNbShapes();
				PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
				physx_actors[i].actorPtr->getShapes(shapes, nbShapes);

				for (PxU32 j = 0; j < nbShapes; j++)
				{
					// Get the geometry of the shape
					const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *physx_actors[i].actorPtr));
					const PxGeometryHolder h = shapes[j]->getGeometry();

					// Generate a mat4 out of the shape position so can send it to the vertex shader
					model = glm::make_mat4(&shapePose.column0.x);

					// check what geometry type the shape is
					if (h.any().getType() == PxGeometryType::eBOX)
					{
						
					}
					else if (h.any().getType() == PxGeometryType::eSPHERE)
					{

					}
					else if (h.any().getType() == PxGeometryType::eCONVEXMESH) {

						if (j == 0) {
							car.Draw(FRWHEEL, shaderProgram, model);;
						}
						else if (j == 1) {
							car.Draw(FLWHEEL, shaderProgram, model);
						}
						else if (j == 2) {
							car.Draw(BRWHEEL, shaderProgram, model);
						}
						else if (j == 3) {
							car.Draw(BLWHEEL, shaderProgram, model);
						}
						else if (j == 4) {
							car.Draw(CHASSIS, shaderProgram, model);
						}
					}
				}
			}
		}
		

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
	}

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

	cleanupPhysics();

	return 0;
}
