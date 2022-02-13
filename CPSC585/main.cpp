 //Initializes the physx system and all global variables and calls other includes.
#include"init.h"



int main()
{
	GraphicsSystem graphics;
	Skybox skybox;

	// DEBUG Panel
	DebugPanel debugPanel(graphics.window);


	//Set up physx with vehicle snippet:
	//Make sure this is called after the shader program is generated
	initPhysics();

	MainMenu mainMenu;
	player.createModel();


	Model press_f_to_rob("models/popups/press_f_to_rob.obj");

	Model* active_level;
	std::vector<Model> levels{
		Model(level_texture_paths[0]),
		Model(level_texture_paths[1]),
		Model(level_texture_paths[2]),
		Model(level_texture_paths[3])
	};
	
	//Test enemy
	Model police_car_chassis(POLICE_CAR_CHASSIS_PATH);
	Model police_car_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_car_rwheel(POLICE_CAR_RWHEEL_PATH);

	CarModel4W police_car(police_car_chassis, police_car_lwheel, police_car_rwheel);

	Vehicle* test_enemy = NULL;


	active_level = &levels[0];

	//Mesh building;
	//building.createBox(bank.getWidth(), bank.getHeight(), bank.getDepth());
	Model bankModel(BANK_BUILDING_PATH);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	glfwSwapInterval(1);


	//////////////////////////
	// CAMERAS 
	//////////////////////////

	// Creates camera pointer
	Camera* activeCamera;
	// Camrea can be one of these at a given time
	BoundCamera boundCamera; // Locked in a sphere around the car
	FreeCamera freeCamera; // Move and look freely anywhere (for debugging)
	// Init to bound camera
	activeCamera = &boundCamera;

	
	std::vector<glm::vec3> light_positions = mainMenu.level_light_positions[0];

	

	// Main while loop
	while (!glfwWindowShouldClose(graphics.window) && !state.terminateProgram)
	{
		//Update the time and fps counter.
		state.updateTime();
		if (state.timeSinceLastFpsUpdate >= 1.0f/30.0f) {
			updateTitle(graphics.window);
			state.prevTime = state.currTime;
			state.timeSinceLastFpsUpdate = 0;
		}

		// Take care of all GLFW events
		glfwPollEvents();


		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use

	
		//Check if in the main menu
		if (state.mainMenu) {
			//Draw the menu
			graphics.shader2D->use();
			mainMenu.level_models[state.selectedMainMenuOption].Draw(*graphics.shader2D);

			checkMainMenuInputs(graphics.window);

			//Despawn any additional active vehicles (enemies)
			while (activeVehicles.size() > 1) {
				despawnEnemy(activeVehicles.back());
				activeVehicles.pop_back();
			}
			
			//If exiting the main menu
			if (!state.mainMenu) {
				//Setup level

				active_level = &levels[state.selectedLevel];
				
				//Remove the old level pointer and add the new
				PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
				gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, gCooking, state.selectedLevel);
				
				gScene->removeActor(*activeLevelActorPtr);
				
				gScene->addActor(*gGroundPlane);

				PxU32 size = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC) * sizeof(PxActor*);
				PxActor** actors = (PxActor**)malloc(size);
				gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, actors, size, 0);
				activeLevelActorPtr = actors[gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC) - 1];

				light_positions = mainMenu.level_light_positions[state.selectedLevel];

				if (state.selectedLevel == 0) {
					test_enemy = new Vehicle(POLICE_CAR, activeVehicles.size(), PxVec3(10.0f, 0.0f, 0.0f));
					activeVehicles.push_back(test_enemy);
				}
				
				//Reset active vehicles
				for (Vehicle* v : activeVehicles) {
					v->reset();
				}
			}
			
		}
		else {

			// view/projection transformations

			glm::mat4 projection = glm::perspective(glm::radians(activeCamera->zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);

			// view/projection transformations
			glm::mat4 view = glm::mat4(glm::mat3(activeCamera->GetViewMatrix())); // remove translation from the view matrix
			skybox.Draw(projection, view);


			view = activeCamera->GetViewMatrix();

			//Tell player if they can rob
			if (player.canRob()) {
				graphics.shader2D->use();
				press_f_to_rob.Draw(*graphics.shader2D);
			}
			graphics.shader3D->use();
			//Simulate physics through the timestep
			stepPhysics(graphics.window);

			//Check for special inputs (currently only camera mode change)
			checkSpecialInputs(graphics.window);

			if (state.cameraMode == CAMERA_MODE_BOUND) activeCamera = &boundCamera;
			else if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) activeCamera = &freeCamera;


			// Camera is disabled in DEBUG MODE
			if (!state.debugMode) activeCamera->handleInput(graphics.window);
			if (activeCamera == &boundCamera) boundCamera.checkClipping(graphics.window);


			// send them to shader
			graphics.shader3D->setMat4("projection", projection);
			graphics.shader3D->setMat4("view", view);

			graphics.shader3D->setInt("numLights", light_positions.size());

			for (int i = 0; i < light_positions.size(); i++) {
				std::string path = "light_positions[" + std::to_string(i) + "]";
				graphics.shader3D->setVec3(path.c_str(), light_positions[i]);
			}

			// render the loaded model

			glm::mat4 model = glm::mat4(1.0f);
			graphics.shader3D->setMat4("model", model);
			graphics.shader3D->setVec3("camPos", glm::vec3(activeCamera->pos.x, activeCamera->pos.y, activeCamera->pos.z));
			graphics.shader3D->setInt("shaderMode", SHADER_MODE_DIFFUSE);
			active_level->Draw(*graphics.shader3D);

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
							//BANK MODEL NOT INCLUDED FOR NOW, BANK IS PART OF GROUND PLANE	
							//Note that the trigger is also of pxGeometryType::eBox now

						}
						else if (h.any().getType() == PxGeometryType::eSPHERE)
						{

						}
						else if (h.any().getType() == PxGeometryType::eCONVEXMESH) {

							graphics.shader3D->setInt("shaderMode", SHADER_MODE_DIFFUSE);
							CarModel4W* activeCar;
							activeCar = player.car;
							if (i != 0) activeCar = &police_car;

							if (j == 0) {
								activeCar->Draw(FRWHEEL, *graphics.shader3D, model);;
							}
							else if (j == 1) {
								activeCar->Draw(FLWHEEL, *graphics.shader3D, model);
							}
							else if (j == 2) {
								activeCar->Draw(BRWHEEL, *graphics.shader3D, model);
							}
							else if (j == 3) {
								activeCar->Draw(BLWHEEL, *graphics.shader3D, model);
							}
							else if (j == 4) {
								activeCar->Draw(CHASSIS, *graphics.shader3D, model);
							}

						}
					}
				}
			}

			// DEBUG MODE
			if (state.debugMode) { // Camera is deactivated
				debugPanel.draw();
			}


		}
		graphics.swapBuffers();
	}

	debugPanel.cleanUp();
	graphics.cleanup();
	cleanupPhysics();

	return EXIT_SUCCESS;
}
