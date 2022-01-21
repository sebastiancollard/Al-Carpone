//Initializes the physx system and all global variables and calls other includes.
#include"init.h"


int main()
{
	//Set up physx with vehicle snippet
	initPhysics();

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
		window = glfwCreateWindow(width, height, "Al Carpone", NULL, NULL);
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
		glViewport(0, 0, width, height);
	}
	
	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Texture
	Texture texture_crate("textures/crate.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture texture_checker("textures/checker.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);


	//Create base meshes
	Mesh box;
	box.createBox(2.5f, 2.f, 5.f);

	Mesh sphere;
	sphere.createSphere(0.5f, 32, 32);

	Mesh plane;
	plane.createPlane(0, 1000, 10);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	freeCamera camera(width, height, glm::vec3(0.f, 2.f, 10.f));




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

		shaderProgram.Activate();
		
		// Handle ball input
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			createDynamic(PxTransform(
				PxVec3(camera.Position.x, camera.Position.y, camera.Position.z)), 
				PxSphereGeometry(4), 
				PxVec3(camera.Orientation.x, camera.Orientation.y, camera.Orientation.z) * 175.0f
			);
		
		//Check for special inputs (currently only camera mode change)
		checkSpecialInputs(window);

		//Update the camera based on its current mode
		if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
			//Player has full control of camera
			camera.Inputs(window);
		}
		else if (state.cameraMode == CAMERA_MODE_BOUND_FREELOOK) {
			camera.Inputs(window);
			//Overwrite the player inputted position with the bound 3d camera position. 
			//Probably not the best way to do this.
			camera.Position = player.getPos() - (player.getDir() * 10.0f) + glm::vec3(0, 5, 0);
		}
		else {
			//Update camera as third person camera behind the car
			camera.Position = player.getPos() - (player.getDir() * 10.0f) + glm::vec3(0, 5, 0);
			camera.Orientation = player.getDir();
		}


		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(90.0f, 0.1f, 1000.0f);

		// render physx shapes
		{
			const int MAX_NUM_ACTOR_SHAPES = 128;
			PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
			for (PxU32 i = 0; i < static_cast<PxU32>(physx_actors.size()); i++)
			{
				const PxU32 nbShapes = physx_actors[i].actorPtr->getNbShapes();
				PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
				physx_actors[i].actorPtr->getShapes(shapes, nbShapes);
				
				for (PxU32 j = 0; j < nbShapes; j++)
				{
					const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *physx_actors[i].actorPtr));
					const PxGeometryHolder h = shapes[j]->getGeometry();
		
					// render object
					glm::mat4 model_matrix = glm::make_mat4(&shapePose.column0.x);
					//glm::mat4 mvp_mat = camera.cameraMatrix * model_matrix;


					if (h.any().getType() == PxGeometryType::eBOX)
					{
						glActiveTexture(GL_TEXTURE0);
						texture_crate.Bind();
						texture_crate.texUnit(shaderProgram, "tex0", 0);
						
						camera.exportMatrix(shaderProgram, "camMatrix");
						glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));

						box.render();
					}
					else if (h.any().getType() == PxGeometryType::eSPHERE)
					{
						glActiveTexture(GL_TEXTURE0);
						texture_checker.Bind();
						texture_checker.texUnit(shaderProgram, "tex0", 0);
		
						camera.exportMatrix(shaderProgram, "camMatrix");
						glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));

						sphere.render();
					}
					else if (h.any().getType() == PxGeometryType::eCONVEXMESH) {

						glActiveTexture(GL_TEXTURE0);

						camera.exportMatrix(shaderProgram, "camMatrix");
						glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));

						if (j > 5) {
							texture_crate.Bind();
							texture_crate.texUnit(shaderProgram, "tex0", 0);
							box.render();
						} 
						else {
							texture_checker.Bind();
							texture_checker.texUnit(shaderProgram, "tex0", 0);
							sphere.render();
						} 
					}
				}
				glm::mat4 model_matrix(1);
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));
				texture_checker.Bind();
				texture_checker.texUnit(shaderProgram, "tex0", 0);
				plane.render();
			}
		}
		
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
	}
	texture_crate.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

	cleanupPhysics();

	return 0;
}