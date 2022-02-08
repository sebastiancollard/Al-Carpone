//Initializes the physx system and all global variables and calls other includes.
#include"init.h"


#define CASH_ROBBED_PER_FRAME 5	//$5 per frame for now?

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

	// DEBUG Panel
	DebugPanel debugPanel = DebugPanel(window);

	//Set up physx with vehicle snippet
	//Make sure this is called after the shader program is generated
	initPhysics();

	//Create base meshes
	//std::vector<Model> models;

	Model car_chassis(CAR_CHASSIS_PATH);
	Model car_lwheel(CAR_LWHEEL_PATH);
	Model car_rwheel(CAR_RWHEEL_PATH);

	CarModel4W car(car_chassis, car_lwheel, car_rwheel);

	//Test enemy
	Model police_car_chassis(POLICE_CAR_CHASSIS_PATH);
	Model police_car_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_car_rwheel(POLICE_CAR_RWHEEL_PATH);

	CarModel4W police_car(police_car_chassis, police_car_lwheel, police_car_rwheel);

	Vehicle police_car_vehicle(POLICE_CAR, PxVec3(10.0f,0.0f,0.0f));

	activeVehicles.push_back(&police_car_vehicle);

	Model groundPlane(ACTIVE_LEVEL_TEXTURED_MODEL_PATH);

	//Mesh building;
	//building.createBox(bank.getWidth(), bank.getHeight(), bank.getDepth());
	Model bankModel(BANK_BUILDING_PATH);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);


	glfwSwapInterval(1);
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
		if (state.timeSinceLastFpsUpdate >= 1.0f/30.0f) {
			updateTitle(window);
			state.prevTime = state.currTime;
			state.timeSinceLastFpsUpdate = 0;
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
		
		// Handle bank robbing
		if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) && (player.canRob())) {
			//std::cout << "Robbing bank...." << std::endl;
			player.addCash(CASH_ROBBED_PER_FRAME);
		}	

		//Check for special inputs (currently only camera mode change)
		checkSpecialInputs(window);

		if (state.cameraMode == CAMERA_MODE_BOUND) activeCamera = &boundCamera;
		else if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) activeCamera = &freeCamera;
		

		// Camera is disabled in DEBUG MODE
		if (!state.debugMode) activeCamera->handleInput(window);
		if (activeCamera == &boundCamera) boundCamera.checkClipping(window);


		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(activeCamera->zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);
		glm::mat4 view = activeCamera->GetViewMatrix();

		//printMat4(projection* view);

		// send them to shader
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		shaderProgram.setMat4("model", model);
		groundPlane.Draw(shaderProgram);

		// Render dynamic physx shapes

		//printf("PITCH[%.2f] | GOAL[%.2f]\nYAW[%.2f] | GOAL[%.2f]\n\n", activeCamera->getPitch(), activeCamera->getPitchGoal(), activeCamera->getYaw(), activeCamera->getYawGoal());
		
		
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
					if (h.any().getType() == PxGeometryType::eBOX)	//Bank is just a box for now.
					{
						//glActiveTexture(GL_TEXTURE0);
						model = scale(model, {bank.getWidth(), bank.getHeight(), bank.getDepth()});
						shaderProgram.setMat4("model", model);
						bankModel.Draw(shaderProgram);

					}
					else if (h.any().getType() == PxGeometryType::eSPHERE)
					{

					}
					else if (h.any().getType() == PxGeometryType::eCONVEXMESH) {
						
						CarModel4W* activeCar;
						activeCar = &car;
						if (i == 3) activeCar = &police_car;

						if (j == 0) {
							activeCar->Draw(FRWHEEL, shaderProgram, model);;
						}
						else if (j == 1) {
							activeCar->Draw(FLWHEEL, shaderProgram, model);
						}
						else if (j == 2) {
							activeCar->Draw(BRWHEEL, shaderProgram, model);
						}
						else if (j == 3) {
							activeCar->Draw(BLWHEEL, shaderProgram, model);
						}
						else if (j == 4) {
							activeCar->Draw(CHASSIS, shaderProgram, model);
						}
					}
				}
			}
		}

		// DEBUG MODE
		if (state.debugMode) { // Camera is deactivated
			debugPanel.draw();
		}

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
	}

	debugPanel.cleanUp();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

	cleanupPhysics();

	return EXIT_SUCCESS;
}
