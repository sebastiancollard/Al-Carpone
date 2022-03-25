 //Initializes the physx system and all global variables and calls other includes.
#include"init.h"
#include"util.h"

namespace sv = snippetvehicle;

extern void renderAll(Camera*, GraphicsSystem*, MainMenu*, Player*, UI*, State*, CarModel4W*, DebugTools);
extern void despawnItem();
extern void checkForItemActions(Player* , Camera* , PhysicsSystem*);


int main()
{
	// TODO clean up
	State state;
	Player player;

	cout << "Initializing Graphics..." << endl;

	GraphicsSystem graphics; //Must be called first ALWAYS

	// Initialize Windows
	cout << "	Debug Panel..." << endl;
	DebugPanel debugPanel(graphics.window);
	cout << "	Main Menu..." << endl;
	MainMenu mainMenu;
	cout << "	Pause Menu..." << endl;
	PauseMenu pauseMenu;
	cout << "	UI..." << endl;
	UI ui;
	
	graphics.clearBuffer();
	mainMenu.drawLoadingGameScreen(graphics);
	graphics.swapBuffers();

	cout << "Initalizing Physics..." << endl;

	//Set up physx with vehicle snippet:
	//Make sure this is called after the shader program is generated
	PhysicsSystem physics(state, player);

	cout << "Initalizing Audio..." << endl;

	AudioSystem audio;

	//TODO Cleanup
	//Setup main player vehicle
	player = Player(0);
	state.playerPtr = &player;
	
	cout << "Initializing AI..." << endl;


	//Setup police and driving nodes
	DrivingNodes* dNodes = new DrivingNodes(
		load_positions("models/map/PATROL_NODES.obj"), load_edges("models/map/PATROL_NODES.obj"),
		load_positions("models/map/NAVIGATION_NODES.obj"), load_edges("models/map/NAVIGATION_NODES.obj"));

	Model police_car_chassis("models/police_car/police_car_chassis.obj");
	Model police_car_lwheel("models/police_car/police_car_wheel_left.obj");
	Model police_car_rwheel("models/police_car/police_car_wheel_right.obj");

	CarModel4W* policeCarModel = new CarModel4W(police_car_chassis, police_car_lwheel, police_car_rwheel);

	createPolice(dNodes, policeCarModel, state);

	DebugTools dTools;
	
	// Build list of buildings
	Bank bank;
	state.buildings[BUILDINGS::BANK] = &bank;

	// engine upgrade variables
	//player.vehiclePtr->mDriveSimData.getGearsData().mSwitchTime;
	//player.vehiclePtr->mDriveSimData.getEngineData().mPeakTorque;

	// robbery upgrade variables
	//bank.robRate;
	//state.activePoliceVehicles[0]->detectionRadius;

	// handling upgrade variables
	//player.vehiclePtr->mWheelsSimData.getSuspensionData(0).mSpringStrength;
	//player.vehiclePtr->mWheelsSimData.getWheelData(0).mMaxSteer;
	//gFrictionPairs->setTypePairFriction(gFrictionPairs->getTypePairFriction() * 1.1f);

	

	Garage robbingGarage(GarageTypes::ROBBING_GARAGE, PxVec3(250, 0, -89.655), PxVec3(19, 2, 19), graphics);
	robbingGarage.menuTitle = Model("models/garageMenu/robbery_upgrades/robbery_upgrades_title.obj");
	Upgrade robSpeed(UPGRADE_TYPE::ROBBERY, UPGRADE_SPECIFIER::ROB_SPEED, 3);
	robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_1.obj"));
	robbingGarage.upgradeList.push_back(robSpeed);
	Upgrade detectionRadius(UPGRADE_TYPE::ROBBERY, UPGRADE_SPECIFIER::DETECTION_RADIUS, 3);
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_1.obj"));
	robbingGarage.upgradeList.push_back(detectionRadius);
	state.buildings[BUILDINGS::GARAGE1] = &robbingGarage;

	Garage handlingGarage(GarageTypes::HANDLING_GARAGE, PxVec3(-100, -20, -278), PxVec3(19, 2, 10), graphics);
	handlingGarage.menuTitle = Model("models/garageMenu/handling_upgrades/handling_upgrades_title.obj");
	Upgrade tireFriction(UPGRADE_TYPE::HANDLING, UPGRADE_SPECIFIER::TIRE_FRICTION, 3);
	tireFriction.menuElements.push_back(Model("models/garageMenu/handling_upgrades/tire_friction_1.obj"));
	handlingGarage.upgradeList.push_back(tireFriction);
	Upgrade steerAngle(UPGRADE_TYPE::HANDLING, UPGRADE_SPECIFIER::TURN_RADIUS, 3);
	steerAngle.menuElements.push_back(Model("models/garageMenu/handling_upgrades/steer_angle_1.obj"));
	handlingGarage.upgradeList.push_back(steerAngle);
	Upgrade suspension(UPGRADE_TYPE::HANDLING, UPGRADE_SPECIFIER::SUSPENSION, 3);
	suspension.menuElements.push_back(Model("models/garageMenu/handling_upgrades/suspension_1.obj"));
	handlingGarage.upgradeList.push_back(suspension);
	state.buildings[BUILDINGS::GARAGE2] = &handlingGarage;


	Garage engineGarage(GarageTypes::ENGINE_GARAGE, PxVec3(862.1, 20, -280.11), PxVec3(29, 2, 29), graphics);
	engineGarage.menuTitle = Model("models/garageMenu/engine_upgrades/engine_upgrades_title.obj");
	Upgrade topSpeed(UPGRADE_TYPE::ENGINE, UPGRADE_SPECIFIER::TOP_SPEED, 3);
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_1.obj"));
	engineGarage.upgradeList.push_back(topSpeed);
	state.buildings[BUILDINGS::GARAGE3] = &engineGarage;
	

	PxFilterData groundPlaneSimFilterData(sv::COLLISION_FLAG_GROUND, sv::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	garageDoor = physics.createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics, gCooking, 1);
	
	CornerStore cornerStore1(PxVec3(-24.470, 0.964, -11.839));
	state.buildings[BUILDINGS::CORNERSTORE1] = &cornerStore1;

	CornerStore cornerStore2(PxVec3(242.158, 0.736, -270.944));
	state.buildings[BUILDINGS::CORNERSTORE2] = &cornerStore2;

	Exit exit;
	state.buildings[BUILDINGS::EXIT] = &exit;

	SelectItem selectItem;



	// Initialize Models
	player.createModel(); //TODO: If player is moved here as well, we can create model in constructors instead.


	graphics.enableDepthBuffer();


	// Active camera can be one of [bound/free] at a given time
	BoundCamera boundCamera(player, state);	// Locked in a sphere around the car
	FreeCamera freeCamera(player);		// Move and look freely anywhere (for debugging)
	Camera* activeCamera = &boundCamera;

	PxTransform T(PxVec3(250.f, 2.0f, -90.f));
	T.q = PxQuat(0.000010, 0.999808, 0.000475, -0.019572);
	player.setResetPoint(T);

	gScene->addActor(*garageDoor);

	state.updateTime();
	state.updateTime(); //flush timestep to fix step()

	spawnPolice(state);



	state.resetVehicles();


	//////////////////////////////////////////////////////////////////////////
	// MAIN LOOP
	/////////////////////////////////////////////////////////////////////////
	
	while (!glfwWindowShouldClose(graphics.window) && !state.terminateProgram)
	{
		// Update the time and fps counter.
		state.updateTime();
		if (state.timeSinceLastFpsUpdate >= 1.0f/30.0f) {
			graphics.updateTitle(state, player);
			state.prevTime = state.currTime;
			state.timeSinceLastFpsUpdate = 0;
		}

		
		glfwPollEvents();			// Take care of all GLFW events
		graphics.clearBuffer();
		audio.updateAudio(&player, &state); // Update all audio elements

		if (state.gameWon) {
			mainMenu.drawWinScreen(graphics);
		}
		else if (state.gameLost) {
			mainMenu.drawLoseScreen(graphics);
			if (glfwGetKey(graphics.window, GLFW_KEY_F) == GLFW_PRESS) {
				if (!state.f_isHeld) {
					
					player.setCash(0);
					player.reset();

					for (PoliceCar* p : state.activePoliceVehicles) {
						p->hardReset();
					}

					state.gamestate = GAMESTATE::GAMESTATE_INGAME;
					state.gameLost = false;
				}
				state.f_isHeld = true;
			}
			else {
				state.f_isHeld = false;
			}
		}

		///////////////////////////////////////////////////////////////
		//MAIN MENU
		///////////////////////////////////////////////////////////////
		else if (state.gamestate == GAMESTATE_MAIN_MENU) {
			//Draw the menu
			mainMenu.drawMenu(graphics, state, &audio);
	
			if (state.gamestate == GAMESTATE::GAMESTATE_INGAME) {
				state.updateTime();
				state.updateTime();
			}

		}

		///////////////////////////////////////////////////////////////
		//corner store
		///////////////////////////////////////////////////////////////
		else if (state.gamestate == GAMESTATE_CORNERSTORE)
		{
			selectItem.drawMenu(graphics, state, player);
		}
		///////////////////////////////////////////////////////////////
		//jail
		///////////////////////////////////////////////////////////////
		
		else if (state.gamestate == GAMESTATE_JAILED)
		{
			mainMenu.drawJailScreen(&graphics);

			if (glfwGetKey(graphics.window, GLFW_KEY_F) == GLFW_PRESS) {
				if (!state.f_isHeld) {
					if (player.getCash() >= 10.0f) {
						player.setCash(player.getCash() - 10.0f);
						//player.setPos();

						PxVec3 p(190.21, 0.96, -194.67);
						PxQuat q(-0.00, -0.71, 0.00, -0.70);

						PxTransform t;
						t.p = p;
						t.q = q;

						player.actorPtr->setGlobalPose(t);
						
						state.gamestate = GAMESTATE::GAMESTATE_INGAME;
					}
					else state.gameLost = true;
				}
				state.f_isHeld = true;
			}
			else {
				state.f_isHeld = false;
			}
		}

		///////////////////////////////////////////////////////////////
		//INGAME
		///////////////////////////////////////////////////////////////
		else {	
			// toggles garage door physx objects
			// must be done before physics.step()
			garageDoorOpen = !player.beingChased(state);
			if (garageDoorOpen && !garageDoorPrev) gScene->removeActor(*garageDoor);
			else if (!garageDoorOpen && garageDoorPrev) gScene->addActor(*garageDoor);
			garageDoorPrev = garageDoorOpen;
		

			//Simulate physics through the timestep
			physics.step(graphics.window);
	

			//Update the players physics variables (velocity, acceleration, jerk)
			//Dont need to check other vehicles (yet?)
			player.updatePhysicsVariables(state.timeStep);

			//updateItem/Powerup information
			player.updatePower();
			if (player.getPower()->shouldDespawn()) {
				despawnItem();
				player.getPower()->setType(NONE);
			}


			//update player jail timer
			player.jailTimer.tick();

			bool playerDetected = false;

			for (PoliceCar* p : state.activePoliceVehicles) {
				if (p->playerDetected) {
					playerDetected = true;
					break;
				}
			}

			if (playerDetected) {
				for (PoliceCar* p : state.activePoliceVehicles) {
					p->startChase();
				}
				if (!debugmode && player.jailTimer.getDeltaTime() > 5.0f) {
					for (PoliceCar* p : state.activePoliceVehicles) {
						p->hardReset();
					}
					player.sendToJail(state);
				}
			}
					
			else player.jailTimer.reset();

			//Check for special inputs (currently only camera mode change)
			checkSpecialInputs(graphics.window, state, player, &audio);

			if (state.cameraMode == CAMERA_MODE_BOUND) activeCamera = &boundCamera;
			else if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) activeCamera = &freeCamera;

			// Camera is disabled in DEBUG MODE
			if (!state.debugMode) activeCamera->handleInput(graphics.window, state);
			if (activeCamera == &boundCamera) boundCamera.checkClipping(graphics.window);


			//Check if player has thrown an item (used a tomato or donut powerup)
			checkForItemActions(&player, &boundCamera, &physics);

			renderAll(activeCamera, &graphics, &mainMenu, &player, &ui,  &state, policeCarModel, dTools);


			// DEBUG MODE
			if (state.debugMode) { // Camera is deactivated
				debugPanel.draw(player);
			}
		}
		graphics.swapBuffers();
	}
	deletePolice(state);
	debugPanel.cleanUp();
	graphics.cleanup();
	physics.cleanup();

	return EXIT_SUCCESS;
}




void renderAll(Camera* activeCamera, GraphicsSystem* graphics, MainMenu* mainMenu, Player* player, UI* ui, State* state, CarModel4W* policeCarModel, DebugTools dTools) {

	glm::mat4 projection = glm::perspective(glm::radians(activeCamera->zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);
	glm::mat4 view = glm::mat4(glm::mat3(activeCamera->GetViewMatrix())); // remove translation from the view matrix
	graphics->skybox->Draw(projection, view);
	view = activeCamera->GetViewMatrix();


	ui->update(state, player, graphics);

	graphics->shader3D->use();
	// send them to shader
	graphics->shader3D->setMat4("projection", projection);
	graphics->shader3D->setMat4("view", view);

	graphics->shader3D->setInt("numLights", mainMenu->light_positions.size());

	for (int i = 0; i < mainMenu->light_positions.size(); i++) {
		std::string path = "light_positions[" + std::to_string(i) + "]";
		graphics->shader3D->setVec3(path.c_str(), mainMenu->light_positions[i]);
	}

	// render the loaded model

	glm::mat4 model = glm::mat4(1.0f);
	graphics->shader3D->setMat4("model", model);
	graphics->shader3D->setVec3("camPos", glm::vec3(activeCamera->pos.x, activeCamera->pos.y, activeCamera->pos.z));
	graphics->shader3D->setInt("shaderMode", SHADER_MODE_DIFFUSE);
	mainMenu->level.Draw(*graphics->shader3D);

	if (!garageDoorOpen) mainMenu->garageDoor.Draw(*graphics->shader3D);

	if (debugmode) {
		for (PoliceCar* p : state->activePoliceVehicles) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, p->getTargetNodeLocation());
			graphics->shader3D->setMat4("model", model);
			dTools.red_node.Draw(*graphics->shader3D);
		}
	}

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
					// Can only render all boxes as one rn
					glActiveTexture(GL_TEXTURE0);
					glUniformMatrix4fv(glGetUniformLocation(graphics->shader3D->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
					//state->buildings[0]->trigger->draw();
					// render a box (hardcoded with same dimensions as physx one) using the model and cam matrix
					//if(police_car->headlights !=  nullptr) police_car->headlights->draw();
				}
				else if (h.any().getType() == PxGeometryType::eSPHERE)
				{
				}
				else if (h.any().getType() == PxGeometryType::eCONVEXMESH) {

					graphics->shader3D->setInt("shaderMode", SHADER_MODE_DIFFUSE);
					CarModel4W* activeCar;
					activeCar = player->car;
					if (i != 0) activeCar = policeCarModel;

					if (j == 0) {
						activeCar->Draw(FRWHEEL, *graphics->shader3D, model);;
					}
					else if (j == 1) {
						activeCar->Draw(FLWHEEL, *graphics->shader3D, model);
					}
					else if (j == 2) {
						activeCar->Draw(BRWHEEL, *graphics->shader3D, model);
					}
					else if (j == 3) {
						activeCar->Draw(BLWHEEL, *graphics->shader3D, model);
					}
					else if (j == 4) {
						activeCar->Draw(CHASSIS, *graphics->shader3D, model);
					}
				}
			}
			//Instead of cycling through all physx actors, this cycles through those that have been added to the 
			//sinple_renderables vector. This is a vector of structs that each contain an actor pointer and a singular model associated
			//with that actor. Is very similar to the above for loop.
			//Future TODO: for more complex objects, add ability to have multiple models per object (like the cars)
			for (auto object : simple_renderables) {

				const PxU32 nbShapes = object.actorPtr->getNbShapes();
				object.actorPtr->getShapes(shapes, nbShapes);

				//assumption that there is only one shape per object in renderables (currently only used for items)
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[0], *object.actorPtr));

				model = glm::make_mat4(&shapePose.column0.x);

				Shader& shader = *graphics->shader3D;
				shader.setMat4("model", model);
				object.model.Draw(*graphics->shader3D);

			}
		}
	}
}

void despawnItem() 
{
	for (int i = 0; i < simple_renderables.size(); i++) 
	{
		if (simple_renderables[i].name == "powerup") 
		{
			PxRigidActor* ptr = simple_renderables[i].actorPtr;
			simple_renderables.erase(simple_renderables.begin() + i);	//erase from simple_renderables

			printf("Erasing item...\n");
			i = i - 1;

			for (int j = 0; j < physx_actors.size(); j++) 
			{		
				if (physx_actors[j].actorPtr == ptr) 
				{
					physx_actors.erase(physx_actors.begin() + j);		//erase from physx_actors
					break;
				}
			}
		}
	}
}

void checkForItemActions(Player* player, Camera* boundCamera, PhysicsSystem* physics) {
	if (player->getPower()->throw_item) {			//PLAYER THROWS ITEM
		player->getPower()->stopThrow();
		PxRigidDynamic* actor;

		if (player->getPower()->getType() == DONUT) {
			actor = physics->createDynamicItem(PxTransform(
				PxVec3(player->getPos().x, (player->getPos().y + 0.8), player->getPos().z)),
				PxBoxGeometry(0.3, 0.2, 0.3),	//donut is box
				PxVec3(boundCamera->dir.x, boundCamera->dir.y, boundCamera->dir.z) * 30.0f		//donut velocity
			);
		}
		else {
			actor = physics->createDynamicItem(PxTransform(
				PxVec3(player->getPos().x, (player->getPos().y + 0.8), player->getPos().z)),
				PxSphereGeometry(0.5),			//tomato is sphere
				PxVec3(boundCamera->dir.x, boundCamera->dir.y, boundCamera->dir.z) * 40.0f		//tomato velocity
			);
		}
		Model model = Model(player->getPower()->getModelPath());
		simple_renderables.push_back({ actor, model, "powerup"});
		//player->getPower()->setType(NONE);

	}
	else if (player->getPower()->drop_item) {		//PLAYER DROPS SPIKE TRAP
		player->getPower()->stopDrop();

		PxRigidDynamic* actor = physics->createDynamicItem(PxTransform(
			PxVec3(player->getPos().x, (player->getPos().y + 0.8), player->getPos().z)),
			PxBoxGeometry(1.2, 0.3, 0.3),		//spike trap is box
			PxVec3((-boundCamera->dir.x), boundCamera->dir.y, (-boundCamera->dir.z)) * 8.0f		//spike velocity
		);
		
		Model model = Model(player->getPower()->getModelPath());
		simple_renderables.push_back({ actor, model , "powerup" });
		//player->getPower()->setType(NONE);

	}
}