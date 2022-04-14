 //Initializes the physx system and all global variables and calls other includes.
#include"init.h"
#include"util.h"

namespace sv = snippetvehicle;

extern void renderAll(Camera*, GraphicsSystem*, MainMenu*, Player*, UI*, State*, CarModel4W*, DebugTools, TextRenderer*, Model* detectionSphere, ItemModels*);
extern void despawnItems();
extern void checkForItemActions(Player* , Camera* , PhysicsSystem*, State*, ItemModels* item_models, PxRigidDynamic* actor);

int main()
{
	// TODO clean up
	State state;
	Player player;

	cout << "Initializing Graphics..." << endl;

	GraphicsSystem graphics; //Must be called first ALWAYS

	// Initialize Windows
	cout << "	Debug Panel..." << endl;
	DebugPanel debugPanel(graphics.window, &state);
	cout << "	Main Menu..." << endl;
	MainMenu mainMenu;
	cout << "	Pause Menu..." << endl;
	PauseMenu pauseMenu;
	cout << "	UI..." << endl;
	UI ui;
	cout << "	FreeType..." << endl;
	TextRenderer text_renderer;
	text_renderer.initFont();

	graphics.clearBuffer();
	mainMenu.drawLoadingGameScreen(graphics);
	graphics.swapBuffers();

	cout << "Initalizing Physics..." << endl;

	//Set up physx with vehicle snippet:
	//Make sure this is called after the shader program is generated
	PhysicsSystem physics(state, player);

	cout << "Initalizing Audio..." << endl;

	AudioSystem audio;
	state.audioSystemPtr = &audio;
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
	Model police_detection_sphere("models/police_car/detection_sphere.obj");

	CarModel4W* policeCarModel = new CarModel4W(police_car_chassis, police_car_lwheel, police_car_rwheel);

	if(debugmode != DEBUGMODE::NOCOPS) createPolice(dNodes, policeCarModel, state);

	DebugTools dTools;

	//Setup powerup models
	Model tomato_model(TOMATO_PATH);
	Model donut_model(DONUT_PATH);
	Model spike_model(SPIKE_PATH);

	physics.setupItemMeshes(&tomato_model, &donut_model, &spike_model);

	//PxRigidDynamic* tomato_actor;
	//PxRigidDynamic* donut_actor;
	//PxRigidDynamic* spike_actor;


	ItemModels* item_models = new ItemModels(tomato_model, donut_model, spike_model);

	// Build list of buildings
	Bank bank;
	state.buildings[BUILDINGS::BANK] = &bank;



	// update player stats to starter stats
	
	// robbery speed done by default
	// detection radius done by default

	// top speed
	PxVehicleEngineData engineData = player.vehiclePtr->mDriveSimData.getEngineData();
	engineData.mPeakTorque = PLAYER_BASE_ENGINE_PEAK_TORQUE;
	player.vehiclePtr->mDriveSimData.setEngineData(engineData);
	// gear speed
	PxVehicleGearsData gearData = player.vehiclePtr->mDriveSimData.getGearsData();
	gearData.mSwitchTime = PLAYER_BASE_GEAR_CHANGE_SPEED;
	player.vehiclePtr->mDriveSimData.setGearsData(gearData);

	// road friction done by default
	// steer angle
	PxVehicleWheelData wheelData = player.vehiclePtr->mWheelsSimData.getWheelData(0);
	wheelData.mMaxSteer = PLAYER_BASE_STEER_ANGLE;
	player.vehiclePtr->mWheelsSimData.setWheelData(0, wheelData);
	wheelData = player.vehiclePtr->mWheelsSimData.getWheelData(1);
	wheelData.mMaxSteer = PLAYER_BASE_STEER_ANGLE;
	player.vehiclePtr->mWheelsSimData.setWheelData(1, wheelData);
	// car flip TODO
	
	Model select1("models/garageMenu/select_1.obj");
	Model select2("models/garageMenu/select_2.obj");
	Model select3("models/garageMenu/select_3.obj");

	Model garageEntrance("models/garageMenu/entrance.obj");

	Garage robbingGarage(GarageTypes::ROBBING_GARAGE, PxVec3(250, 0, -89.655), PxVec3(19, 2, 19), graphics);
	robbingGarage.menuTitle = Model("models/garageMenu/robbery_upgrades/robbery_upgrades_title.obj");
	robbingGarage.selectTop = select1;
	robbingGarage.selectMid = select2;
	robbingGarage.selectBot = select3;
	//Upgrade robSpeed(UPGRADE_TYPE::ROBBERY, UPGRADE_SPECIFIER::ROB_SPEED, 5);
	//robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_1.obj"));
	//robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_2.obj"));
	//robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_3.obj"));
	//robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_4.obj"));
	//robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_5.obj"));
	//robSpeed.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/rob_speed_maxed.obj"));
	//robSpeed.upgradeInfo.clear();
	//robSpeed.upgradeInfo.push_back(std::make_tuple(1.25f, true, 5000));
	//robSpeed.upgradeInfo.push_back(std::make_tuple(2.5f, true, 11250));
	//robSpeed.upgradeInfo.push_back(std::make_tuple(5.f, true, 17500));
	//robSpeed.upgradeInfo.push_back(std::make_tuple(10.f, true, 30000));
	//robSpeed.upgradeInfo.push_back(std::make_tuple(20.f, true, 55000));
	//robbingGarage.upgradeList.push_back(robSpeed);
	Upgrade alarmChance(UPGRADE_TYPE::ROBBERY, UPGRADE_SPECIFIER::ALARM_CHANCE, 2);
	alarmChance.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/alarm_chance_1.obj"));
	alarmChance.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/alarm_chance_2.obj"));
	alarmChance.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/alarm_chance_maxed.obj"));
	alarmChance.upgradeInfo.clear();
	alarmChance.upgradeInfo.push_back(std::make_tuple(0.25f, true, 5625));
	alarmChance.upgradeInfo.push_back(std::make_tuple(0.5f, true, 15000));
	robbingGarage.upgradeList.push_back(alarmChance);
	Upgrade detectionRadius(UPGRADE_TYPE::ROBBERY, UPGRADE_SPECIFIER::DETECTION_RADIUS, 5);
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_1.obj"));
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_2.obj"));
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_3.obj"));
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_4.obj"));
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_5.obj"));
	detectionRadius.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/detection_radius_maxed.obj"));
	detectionRadius.upgradeInfo.clear();
	detectionRadius.upgradeInfo.push_back(std::make_tuple(0.1f, true, 2500));
	detectionRadius.upgradeInfo.push_back(std::make_tuple(0.2f, true, 2500));
	detectionRadius.upgradeInfo.push_back(std::make_tuple(0.3f, true, 5625));
	detectionRadius.upgradeInfo.push_back(std::make_tuple(0.4f, true, 5625));
	detectionRadius.upgradeInfo.push_back(std::make_tuple(0.5f, true, 15000));
	robbingGarage.upgradeList.push_back(detectionRadius);
	Upgrade policeOnMap(UPGRADE_TYPE::ROBBERY, UPGRADE_SPECIFIER::MINIMAP, 3);
	policeOnMap.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/enhanced_minimap_1.obj"));
	policeOnMap.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/enhanced_minimap_2.obj"));
	policeOnMap.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/enhanced_minimap_3.obj"));
	policeOnMap.menuElements.push_back(Model("models/garageMenu/robbery_upgrades/enhanced_minimap_maxed.obj"));
	policeOnMap.upgradeInfo.clear();
	policeOnMap.upgradeInfo.push_back(std::make_tuple(0.f, false, 2500));
	policeOnMap.upgradeInfo.push_back(std::make_tuple(200.f, false, 5625));
	policeOnMap.upgradeInfo.push_back(std::make_tuple(0.f, false, 15000));
	//policeOnMap.upgradeInfo.push_back(std::make_tuple(10000.f, false, 15000));
	robbingGarage.upgradeList.push_back(policeOnMap);
	state.buildings[BUILDINGS::GARAGE1] = &robbingGarage;

	Garage handlingGarage(GarageTypes::HANDLING_GARAGE, PxVec3(-100, -20, -278), PxVec3(19, 2, 10), graphics);
	handlingGarage.menuTitle = Model("models/garageMenu/handling_upgrades/handling_upgrades_title.obj");
	handlingGarage.selectTop = select1;
	handlingGarage.selectMid = select2;
	handlingGarage.selectBot = select3;
	Upgrade roadFriction(UPGRADE_TYPE::HANDLING, UPGRADE_SPECIFIER::TIRE_FRICTION, 3);
	roadFriction.upgradeInfo.clear();
	roadFriction.upgradeInfo.push_back(std::make_tuple(0.1f, true, 2500));
	roadFriction.upgradeInfo.push_back(std::make_tuple(0.2f, true, 5625));
	roadFriction.upgradeInfo.push_back(std::make_tuple(0.3f, true, 15000));
	roadFriction.menuElements.push_back(Model("models/garageMenu/handling_upgrades/tire_friction_1.obj"));
	roadFriction.menuElements.push_back(Model("models/garageMenu/handling_upgrades/tire_friction_2.obj"));
	roadFriction.menuElements.push_back(Model("models/garageMenu/handling_upgrades/tire_friction_3.obj"));
	roadFriction.menuElements.push_back(Model("models/garageMenu/handling_upgrades/tire_friction_maxed.obj"));
	handlingGarage.upgradeList.push_back(roadFriction);
	Upgrade steerAngle(UPGRADE_TYPE::HANDLING, UPGRADE_SPECIFIER::TURN_RADIUS, 2);
	steerAngle.upgradeInfo.clear();
	steerAngle.upgradeInfo.push_back(std::make_tuple(1.f / 3.f, true, 2500));
	steerAngle.upgradeInfo.push_back(std::make_tuple(0.9999f, true, 15000));
	steerAngle.menuElements.push_back(Model("models/garageMenu/handling_upgrades/steer_angle_1.obj"));
	steerAngle.menuElements.push_back(Model("models/garageMenu/handling_upgrades/steer_angle_2.obj"));
	steerAngle.menuElements.push_back(Model("models/garageMenu/handling_upgrades/steer_angle_maxed.obj"));
	handlingGarage.upgradeList.push_back(steerAngle);
	Upgrade flipOver(UPGRADE_TYPE::HANDLING, UPGRADE_SPECIFIER::CAR_FLIP, 1);
	flipOver.upgradeInfo.clear();
	flipOver.upgradeInfo.push_back(std::make_tuple(0.f, true, 5625));
	flipOver.menuElements.push_back(Model("models/garageMenu/handling_upgrades/car_flipper.obj"));
	flipOver.menuElements.push_back(Model("models/garageMenu/handling_upgrades/car_flipper_maxed.obj"));
	handlingGarage.upgradeList.push_back(flipOver);
	state.buildings[BUILDINGS::GARAGE2] = &handlingGarage;

	Garage engineGarage(GarageTypes::ENGINE_GARAGE, PxVec3(862.1, 20, -280.11), PxVec3(29, 2, 29), graphics);
	engineGarage.menuTitle = Model("models/garageMenu/engine_upgrades/engine_upgrades_title.obj");
	engineGarage.selectTop = select1;
	engineGarage.selectMid = select2;
	engineGarage.selectBot = select3;
	Upgrade topSpeed(UPGRADE_TYPE::ENGINE, UPGRADE_SPECIFIER::TOP_SPEED, 5);
	topSpeed.upgradeInfo.clear();
	topSpeed.upgradeInfo.push_back(std::make_tuple(0.1f, true, 2500));
	topSpeed.upgradeInfo.push_back(std::make_tuple(0.2f, true, 2500));
	topSpeed.upgradeInfo.push_back(std::make_tuple(0.3f, true, 5625));
	topSpeed.upgradeInfo.push_back(std::make_tuple(0.4f, true, 15000));
	topSpeed.upgradeInfo.push_back(std::make_tuple(0.5f, true, 15000));
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_1.obj"));
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_2.obj"));
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_3.obj"));
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_4.obj"));
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_5.obj"));
	topSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/top_speed_maxed.obj"));
	engineGarage.upgradeList.push_back(topSpeed);
	Upgrade gearSpeed(UPGRADE_TYPE::ENGINE, UPGRADE_SPECIFIER::GEAR_SPEED, 3);
	gearSpeed.upgradeInfo.clear();
	gearSpeed.upgradeInfo.push_back(std::make_tuple(1.f / 9.f, true, 2500));
	gearSpeed.upgradeInfo.push_back(std::make_tuple(2.f / 9.f, true, 5625));
	gearSpeed.upgradeInfo.push_back(std::make_tuple(3.f / 9.f, true, 15000));
	gearSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/gear_shift_speed_1.obj"));
	gearSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/gear_shift_speed_2.obj"));
	gearSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/gear_shift_speed_3.obj"));
	gearSpeed.menuElements.push_back(Model("models/garageMenu/engine_upgrades/gear_shift_speed_maxed.obj"));
	engineGarage.upgradeList.push_back(gearSpeed);
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
	//ChangePlaylist playlist;


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

	if(debugmode != DEBUGMODE::NOCOPS) spawnPolice(state);



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

					// reset garage upgrades
					for (int i = BUILDINGS::GARAGE1; i <= BUILDINGS::GARAGE3; i++) {
						for (Upgrade& u : ((Garage*)state.buildings[i])->upgradeList)
							u.tier = 0;
					}
					
					// reset player stats
					// alarm chance
					player.baseAlarmChancePerCheck = player.initAlarmChancePerCheck;
					
					// detection radius
					for (PoliceCar* p : state.activePoliceVehicles) {
						p->detectionRadius = p->baseDetectionRadius;
					}
					
					// enhanced map
					player.minimapMode = 0;

					// top speed
					engineData = player.vehiclePtr->mDriveSimData.getEngineData();
					engineData.mPeakTorque = PLAYER_BASE_ENGINE_PEAK_TORQUE;
					player.vehiclePtr->mDriveSimData.setEngineData(engineData);
					// gear speed
					gearData = player.vehiclePtr->mDriveSimData.getGearsData();
					gearData.mSwitchTime = PLAYER_BASE_GEAR_CHANGE_SPEED;
					player.vehiclePtr->mDriveSimData.setGearsData(gearData);

					
					// road friction 
					float rec = 1.f + ((float)timesUpgradedFriction / 10.f);
					gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) * (1.f / rec));
					timesUpgradedFriction = 0;

					// steer angle
					wheelData = player.vehiclePtr->mWheelsSimData.getWheelData(0);
					wheelData.mMaxSteer = PLAYER_BASE_STEER_ANGLE;
					player.vehiclePtr->mWheelsSimData.setWheelData(0, wheelData);
					wheelData = player.vehiclePtr->mWheelsSimData.getWheelData(1);
					wheelData.mMaxSteer = PLAYER_BASE_STEER_ANGLE;
					player.vehiclePtr->mWheelsSimData.setWheelData(1, wheelData);
					// car flip 
					player.canFlip = false;

					state.gamestate = GAMESTATE::GAMESTATE_INGAME;
					state.gameLost = false;
				}
				state.f_isHeld = true;
			}
			else {
				state.f_isHeld = false;
			}

			if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
			{
				bool accel = false;
				//get controller name
				//const char* controller_name = glfwGetGamepadName(GLFW_JOYSTICK_1);
				//std::cout << controller_name << std::endl;

				GLFWgamepadstate controlState;
				if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controlState))
				{
					if (controlState.buttons[GLFW_GAMEPAD_BUTTON_SQUARE])
					{
							if (!state.square_isHeld) {

								player.setCash(0);
								player.reset();

								for (PoliceCar* p : state.activePoliceVehicles) {
									p->hardReset();
								}

								// reset garage upgrades
								for (int i = BUILDINGS::GARAGE1; i <= BUILDINGS::GARAGE3; i++) {
									for (Upgrade& u : ((Garage*)state.buildings[i])->upgradeList)
										u.tier = 0;
								}

								// reset player stats
								// alarm chance
								player.baseAlarmChancePerCheck = player.initAlarmChancePerCheck;

								// detection radius
								for (PoliceCar* p : state.activePoliceVehicles) {
									p->detectionRadius = p->baseDetectionRadius;
								}

								// enhanced map
								player.minimapMode = 0;

								// top speed
								engineData = player.vehiclePtr->mDriveSimData.getEngineData();
								engineData.mPeakTorque = PLAYER_BASE_ENGINE_PEAK_TORQUE;
								player.vehiclePtr->mDriveSimData.setEngineData(engineData);
								// gear speed
								gearData = player.vehiclePtr->mDriveSimData.getGearsData();
								gearData.mSwitchTime = PLAYER_BASE_GEAR_CHANGE_SPEED;
								player.vehiclePtr->mDriveSimData.setGearsData(gearData);


								// road friction 
								float rec = 1.f + ((float)timesUpgradedFriction / 10.f);
								gFrictionPairs->setTypePairFriction(0, 0, gFrictionPairs->getTypePairFriction(0, 0) * (1.f / rec));
								timesUpgradedFriction = 0;

								// steer angle
								wheelData = player.vehiclePtr->mWheelsSimData.getWheelData(0);
								wheelData.mMaxSteer = PLAYER_BASE_STEER_ANGLE;
								player.vehiclePtr->mWheelsSimData.setWheelData(0, wheelData);
								wheelData = player.vehiclePtr->mWheelsSimData.getWheelData(1);
								wheelData.mMaxSteer = PLAYER_BASE_STEER_ANGLE;
								player.vehiclePtr->mWheelsSimData.setWheelData(1, wheelData);
								// car flip 
								player.canFlip = false;

								state.gamestate = GAMESTATE::GAMESTATE_INGAME;
								state.gameLost = false;
							}
							state.square_isHeld = true;
						}
						else {
						state.square_isHeld = false;
					}
					
				}
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
		//else if (state.gamestate == GAMESTATE_CORNERSTORE)
		//{
		//	selectItem.drawMenu(graphics, state, player);
		//	
		//}
		///////////////////////////////////////////////////////////////
		//jail
		///////////////////////////////////////////////////////////////
		
		else if (state.gamestate == GAMESTATE_JAILED)
		{
			mainMenu.drawJailScreen(&graphics);

			if (glfwGetKey(graphics.window, GLFW_KEY_F) == GLFW_PRESS) {
				
				float bail_cost = 2000.f;
				if (player.getCash() * 0.75 > bail_cost) bail_cost = player.getCash() * 0.75;

				if (!state.f_isHeld) {
					if (player.getCash() >= bail_cost) {
						player.setCash(player.getCash() - bail_cost);
						audio.playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
						//player.setPos();
						PxVec3 p(190.21, 0.96, -194.67);
						PxQuat q(-0.00, -0.71, 0.00, -0.70);

						PxTransform t;
						t.p = p;
						t.q = q;

						player.actorPtr->setGlobalPose(t);
						
						state.gamestate = GAMESTATE::GAMESTATE_INGAME;
					}
					else {
						state.gameLost = true;
						audio.playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
						audio.playSoundEffect(SOUND_SELECTION::LOSEGAME);
					}
				}
				state.f_isHeld = true;
			}
			else {
				state.f_isHeld = false;
			}
			if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
			{
				bool accel = false;
				//get controller name
				//const char* controller_name = glfwGetGamepadName(GLFW_JOYSTICK_1);
				//std::cout << controller_name << std::endl;

				GLFWgamepadstate controlState;
				if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controlState))
				{
					if (controlState.buttons[GLFW_GAMEPAD_BUTTON_SQUARE])
					{
						float bail_cost = 2000.f;
						if (player.getCash() * 0.75 > bail_cost) bail_cost = player.getCash() * 0.75;

						if (!state.square_isHeld) {

							if (player.getCash() >= bail_cost) {
								player.setCash(player.getCash() - bail_cost);
								audio.playSoundEffect(SOUND_SELECTION::PURCHASE_SUCCESS);
								//player.setPos();
								PxVec3 p(190.21, 0.96, -194.67);
								PxQuat q(-0.00, -0.71, 0.00, -0.70);

								PxTransform t;
								t.p = p;
								t.q = q;

								player.actorPtr->setGlobalPose(t);

								state.gamestate = GAMESTATE::GAMESTATE_INGAME;
							}
							else {
								state.gameLost = true;
								audio.playSoundEffect(SOUND_SELECTION::PURCHASE_FAIL);
								audio.playSoundEffect(SOUND_SELECTION::LOSEGAME);
							}
						}
						state.square_isHeld = true;
					}
					else {
						state.square_isHeld = false;
					}

				}
			}
		}

		///////////////////////////////////////////////////////////////
		//INGAME
		///////////////////////////////////////////////////////////////
		else {	
			// toggles garage door physx objects
			// must be done before physics.step()
			bool policeAlerted = player.beingChased(state);
			garageDoorOpen = !policeAlerted;
			if (garageDoorOpen && !garageDoorPrev) gScene->removeActor(*garageDoor);
			else if (!garageDoorOpen && garageDoorPrev) gScene->addActor(*garageDoor);
			garageDoorPrev = garageDoorOpen;
			////////////////
			//radio switch//
			////////////////
			//if (state.tab_isHeld || state.right_thumb) { playlist.drawMenu(graphics, state, &audio); }
			////////////////
			//corner store//
			////////////////
			if (player.canChooseTool(state) && (state.buildings[BUILDINGS::CORNERSTORE1]->isInRange 
				|| state.buildings[BUILDINGS::CORNERSTORE2]->isInRange))
			{ selectItem.drawMenu(graphics, state, player); }

			//Simulate physics through the timestep
			physics.step(graphics.window);
	

			//Update the players physics variables (velocity, acceleration, jerk)
			//Dont need to check other vehicles (yet?)
			player.updatePhysicsVariables(state.timeStep);

			//updateItem/Powerup information
			for (auto& p : active_items) {			//update all active powers
				p.updateTimed();

				if (p.shouldDespawn()) {
					if (p.getType() == CAMOUFLAGE)
						player.setDetectable(true);
				}
			}
			if (player.getPower()->isActive()) {	//update currently equipped power (mostly for outputting correct data on the UI)
				player.getPower()->updateTimed();
				if (player.getPower()->shouldDespawn()) {
					player.getPower()->reset();
				}
			}
			despawnItems();



			bool shouldArrest = false;
			int num_arresters = 0;

			for (PoliceCar* p : state.activePoliceVehicles) {
				if (p->playerArrestable) {
					shouldArrest = true;
					num_arresters++;
				}
			}


			if (shouldArrest) {

				if (abs(player.getForwardVelocity()) < 2.f) {
					player.jailTimer += state.timeStep * (float)num_arresters;
				}

				if (debugmode != DEBUGMODE::NOJAIL && player.jailTimer >= 5.0f) {

					player.sendToJail(state);
					audio.playSoundEffect(SOUND_SELECTION::JAIL_DOOR);

					for (PoliceCar* p : state.activePoliceVehicles) {
						p->hardReset();
					}
					active_items.clear();
					player.getPower()->reset();
				}
			}
					
			else {
				player.jailTimer -= state.timeStep * 0.1f;
				if (player.jailTimer < 0) player.jailTimer = 0;
			}

			//printf("JAILTIMER: %.2f\n", player.jailTimer);

			// handle when player is flipped over and doesnt have the upgrade
			if (player.isFlippedOver() && !player.canFlip) {
				player.flippedOverTimer += state.timeStep;
			}
			else if (!player.isFlippedOver() && player.flippedOverTimer != 0.f) player.flippedOverTimer = 0.f;
			if (player.flippedOverTimer > 5.f) player.sendToJail(state);

			// camera mode
			if (state.cameraMode == CAMERA_MODE_BOUND) activeCamera = &boundCamera;
			else if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) activeCamera = &freeCamera;

			// Camera is disabled in DEBUG MODE
			if (!state.debugMode) activeCamera->handleInput(graphics.window, state);
			if (activeCamera == &boundCamera) boundCamera.checkClipping(graphics.window);

			//Check for special inputs (currently only camera mode change)
			// NOTE: needs to be put AFTER camera mode and input checking to prevent camera glitches on certain inputs
			checkSpecialInputs(&graphics, state, player, &audio);

			//Check if player has thrown an item (used a tomato or donut powerup)
			checkForItemActions(&player, &boundCamera, &physics, &state, item_models, donut_actor);

			renderAll(activeCamera, &graphics, &mainMenu, &player, &ui,  &state, policeCarModel, dTools, &text_renderer, &police_detection_sphere, item_models);


			// DEBUG MODE
			if (state.debugMode) { // Camera is deactivated
				debugPanel.draw(player);
				if (debugPanel.enableDebug) debugmode = DEBUGMODE::NOALERT;
				else if (!debugPanel.enableDebug) debugmode = DEBUGMODE::FALSE;
			}
		}
		
		graphics.swapBuffers();
	}
	if(debugmode != DEBUGMODE::NOCOPS) deletePolice(state);
	debugPanel.cleanUp();
	graphics.cleanup();
	physics.cleanup();

	return EXIT_SUCCESS;
}




void renderAll(Camera* activeCamera, GraphicsSystem* graphics, MainMenu* mainMenu, Player* player, UI* ui, State* state, CarModel4W* policeCarModel, DebugTools dTools, TextRenderer* text_renderer, Model* detectionSphere, ItemModels* item_models) {

	glm::mat4 projection = glm::perspective(glm::radians(activeCamera->zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);
	glm::mat4 view = glm::mat4(glm::mat3(activeCamera->GetViewMatrix())); // remove translation from the view matrix
	graphics->skybox->Draw(projection, view);
	view = activeCamera->GetViewMatrix();

	graphics->shader3D->use();
	// send them to shader
	graphics->shader3D->setMat4("projection", projection);
	graphics->shader3D->setMat4("view", view);

	graphics->shader3D->setInt("numLights", mainMenu->light_positions.size());

	for (int i = 0; i < mainMenu->light_positions.size(); i++) {
		std::string path = "light_positions[" + std::to_string(i) + "]";
		graphics->shader3D->setVec3(path.c_str(), mainMenu->light_positions[i]);
	}


	//HEADLIGHTS&&BRAKELIGHTS::

	{
		graphics->shader3D->setInt("numHeadlights",state->activePoliceVehicles.size() * 2 + 4);
		graphics->shader3D->setInt("numBrakelights", state->activePoliceVehicles.size() * 2 + 4);


		float player_brakelight_FOFF = -3.0f;
		float player_brakelight_VOFF = 0.6;

		float player_brakelight1_OOFF = 0.72;
		float player_brakelight2_OOFF = 0.47;
		float player_brakelight3_OOFF = -0.72;
		float player_brakelight4_OOFF = -0.47;


		float police_car_brakelight_FOFF = -2.75;
		float police_car_brakelight_VOFF = 0.089;

		float police_car_brakelight1_OOFF = 0.788;
		float police_car_brakelight2_OOFF = -0.788;

		int index = 0;
		
		if (player->toggleHeadlights) {

			if(player->footOnBrake()) graphics->shader3D->setFloat("brakelightMultiplier", 3.5f);
			else graphics->shader3D->setFloat("brakelightMultiplier", 1.0f);

			std::pair<std::pair<glm::vec3, glm::vec3>, std::pair<glm::vec3, glm::vec3>> headlights = player->getHeadlightPositions();
			std::pair<glm::vec3, glm::vec3> set_1 = headlights.first;
			glm::vec3 set_1_l = set_1.first;
			glm::vec3 set_1_r = set_1.second;
			std::pair<glm::vec3, glm::vec3> set_2 = headlights.second;
			glm::vec3 set_2_l = set_2.first;
			glm::vec3 set_2_r = set_2.second;

			glm::vec3 headlightDirection = player->getDir();

			glm::vec3 origin = player->getPos();
			glm::vec3 uFront = player->getDir();
			glm::vec3 uUp = player->getUp();
			glm::vec3 uRight = player->getRight();

			glm::vec3 lBrakelight1 =
				origin +
				uFront * player_brakelight_FOFF +
				uRight * player_brakelight1_OOFF +
				uUp * player_brakelight_VOFF;
			glm::vec3 lBrakelight2 =
				origin +
				uFront * player_brakelight_FOFF +
				uRight * player_brakelight2_OOFF +
				uUp * player_brakelight_VOFF;
			glm::vec3 rBrakelight1 =
				origin +
				uFront * player_brakelight_FOFF +
				uRight * player_brakelight3_OOFF +
				uUp * player_brakelight_VOFF;
			glm::vec3 rBrakelight2 =
				origin +
				uFront * player_brakelight_FOFF +
				uRight * player_brakelight4_OOFF +
				uUp * player_brakelight_VOFF;

			glm::vec3 brakelightDirection = -player->getDir() - state->brakelight_down_angle * player->getUp();

			index = 0;

			std::string path = "headlight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), set_1_l);
			path = "headlight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), headlightDirection);

			path = "brakelight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), rBrakelight1);
			path = "brakelight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), brakelightDirection);

			index++;

			path = "headlight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), set_1_r);
			path = "headlight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), headlightDirection);

			path = "brakelight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), rBrakelight2);
			path = "brakelight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), brakelightDirection);

			index++;

			path = "headlight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), set_2_l);
			path = "headlight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), headlightDirection);

			path = "brakelight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), lBrakelight1);
			path = "brakelight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), brakelightDirection);


			index++;

			path = "headlight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), set_2_r);
			path = "headlight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), headlightDirection);

			path = "brakelight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), lBrakelight2);
			path = "brakelight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), brakelightDirection);



		}

		
		for (PoliceCar* v : state->activePoliceVehicles) {
			std::pair<glm::vec3, glm::vec3> headlights = v->getHeadlightPositions(state);
			glm::vec3 headlightDirection = v->getDir() - 0.2f * v->getUp();
			glm::vec3 lHeadlight = headlights.first;
			glm::vec3 rHeadlight = headlights.second;

			glm::vec3 origin = v->getPos();
			glm::vec3 uFront = v->getDir();
			glm::vec3 uUp = v->getUp();
			glm::vec3 uRight = v->getRight();

			glm::vec3 lBrakelight =
				origin +
				uFront * police_car_brakelight_FOFF +
				uRight * police_car_brakelight1_OOFF +
				uUp * police_car_brakelight_VOFF;
			glm::vec3 rBrakelight =
				origin +
				uFront * police_car_brakelight_FOFF +
				uRight * police_car_brakelight2_OOFF +
				uUp * police_car_brakelight_VOFF;

			glm::vec3 brakelightDirection = -v->getDir() - state->brakelight_down_angle * v->getUp();

			std::string path = "headlight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), lHeadlight);
			path = "headlight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), headlightDirection);

			path = "brakelight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), lBrakelight);
			path = "brakelight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), brakelightDirection);


			index++;

			path = "headlight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), rHeadlight);
			path = "headlight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), headlightDirection);

			path = "brakelight_positions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), rBrakelight);
			path = "brakelight_directions[" + std::to_string(index) + "]";
			graphics->shader3D->setVec3(path.c_str(), brakelightDirection);

			index++;


		}

	}


	//::HEADLIGHTS&&BRAKELIGHTS

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	graphics->shader3D->setMat4("model", model);
	graphics->shader3D->setVec3("camPos", glm::vec3(activeCamera->pos.x, activeCamera->pos.y, activeCamera->pos.z));
	graphics->shader3D->setInt("shaderMode", SHADER_MODE_DIFFUSE);
	mainMenu->level.Draw(*graphics->shader3D);

	if (!garageDoorOpen) mainMenu->garageDoor.Draw(*graphics->shader3D);

	if (debugmode == DEBUGMODE::NOJAIL) {
		for (PoliceCar* p : state->activePoliceVehicles) {
			model = glm::mat4(1.0f);
			glm::vec3 pos = p->getTargetNodeLocation();
			pos.y = 0;
			model = glm::translate(model, pos);
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
						activeCar->Draw(FRWHEEL, *graphics->shader3D, model);
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
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
						activeCar->Draw(CHASSIS, *graphics->shader3D, model);
						// handle detection sphere render
						if (state->activePoliceVehicles.size() > 0 && activeCar == policeCarModel && !player->beingChased(*state)) {
							
							model = glm::scale(model, glm::vec3(state->activePoliceVehicles[0]->detectionRadius-1));
							graphics->shaderDetection->use();
							graphics->shaderDetection->setMat4("model", model);
							graphics->shaderDetection->setMat4("projection", projection);
							graphics->shaderDetection->setMat4("view", view);
							graphics->shaderDetection->setFloat("detectionRadius", (state->activePoliceVehicles[0]->detectionRadius));
							graphics->shaderDetection->setFloat("distance", glm::distance(player->getPos(), glm::vec3(model[3].x, model[3].y, model[3].z)));
							detectionSphere->Draw(*graphics->shaderDetection);
							graphics->shader3D->use();
						}
						glDisable(GL_BLEND);
					}
				}
			}
			//Instead of cycling through all physx actors, this cycles through those that have been added to the 
			//simple_renderables vector. This is a vector of structs that each contain an actor pointer and a singular model associated
			//with that actor. Is very similar to the above for loop.
			//Future TODO: for more complex objects, add ability to have multiple models per object (like the cars)
			for (auto &item : active_items) {

				if (item.getType() != CAMOUFLAGE) {
					const PxU32 nbShapes = item.actorPtr->getNbShapes();
					item.actorPtr->getShapes(shapes, nbShapes);

					//assumption that there is only one shape per object in renderables (currently only used for items)
					const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[0], *item.actorPtr));

					model = glm::make_mat4(&shapePose.column0.x);

					Shader& shader = *graphics->shader3D;
					shader.setMat4("model", model);
					//object.model.Draw(*graphics->shader3D);
					item_models->Draw(item.getType(), *graphics->shader3D, model);
				}

			}
		}
	}
	
	// UI needs to be drawn after all 3D elements
	ui->update(state, player, graphics, text_renderer);
}

void despawnItems() 
{
	for (int i = 0; i < active_items.size(); i++)
	{
		if (active_items[i].getRemainingTime() <= 0.f)
		{
			active_items.erase(active_items.begin() + i);	//erase from simple_renderables

			printf("Erasing item...\n");
			i = i - 1;
		}
	}
}

void checkForItemActions(Player* player, Camera* boundCamera, PhysicsSystem* physics, State* state, ItemModels* item_models, PxRigidDynamic* donut_actor) {
	PowerUp power;
	PxRigidDynamic* actor;

	// -- PLAYER THROWS ITEM --
	if (player->getPower()->throw_item) {	
		state->audioSystemPtr->playSoundEffect(SOUND_SELECTION::THROW_OUT);
		player->getPower()->stopThrow();

		if (player->getPower()->getType() == DONUT) {		//PLAYER THROWS DONUT
			actor = physics->createDynamicItem(
				POWER_TYPE::DONUT,
				PxTransform(PxVec3(player->getPos().x, (player->getPos().y + 0.8), player->getPos().z)),
				PxVec3(boundCamera->dir.x, boundCamera->dir.y, boundCamera->dir.z) * 30.0f		//donut velocity
			);
		}
		else {
			actor = physics->createDynamicItem(				//PLAYER THROWS TOMATO
				POWER_TYPE::TOMATO,
				PxTransform(PxVec3(player->getPos().x, (player->getPos().y + 0.8), player->getPos().z)),
				PxVec3(boundCamera->dir.x, boundCamera->dir.y, boundCamera->dir.z) * 40.0f		//tomato velocity
			);
			
		}
		
		player->getPower()->actorPtr = actor;
		player->getPower()->itemInWorld = true;
		
		power = *(player->getPower());
		active_items.push_back(power);	//include tomato or donut in active_items


	}
	// -- PLAYER DROPS ITEM -- 
	else if (player->getPower()->drop_item) {
		player->getPower()->stopDrop();

		actor = physics->createDynamicItem(		//PLAYER DROPS SPIKE TRAP
			POWER_TYPE::TOMATO,
			PxTransform(PxVec3(player->getPos().x, (player->getPos().y + 0.8), player->getPos().z)),
			PxVec3((-boundCamera->dir.x), boundCamera->dir.y, (-boundCamera->dir.z)) * 8.0f		//spike velocity
		);

		player->getPower()->actorPtr = actor;
		player->getPower()->itemInWorld = true;
		
		power = *(player->getPower());
		active_items.push_back(power);	//include spike_trap in active_items

	}
	// -- PLAYER BECOMES CAMOUFLAGED --
	else if (!player->isDetectable() && player->getCurrentModelType() == AL_CARPONE) {	
		player->setCurrentModel(POLICE_CAR);
		
		power = *(player->getPower());
		active_items.push_back(power);	//include camouflage in active_items
	}
	
	// -- PLAYER BECOMES UN-CAMOUFLAGED
	if (player->isDetectable() && player->getCurrentModelType() == POLICE_CAR) {	
		player->setCurrentModel(AL_CARPONE);
	}



}