#include "PoliceCar.h"
#include "Player.h"

#define POLICE_CAR_CHASSIS_PATH "models/police_car/police_car_chassis.obj"
#define POLICE_CAR_LWHEEL_PATH "models/police_car/police_car_wheel_left.obj"
#define POLICE_CAR_RWHEEL_PATH "models/police_car/police_car_wheel_right.obj"

// TODO not yet in seconds
#define MAX_IDLE_TIME 20		
#define MAX_CHASE_TIME 20


// Car Speeds
#define PATROL_ACCEL 0.625f
#define CHASE_ACCEL 0.8f


// AI Patrolling nodes
std::vector<std::vector<glm::vec3>> ai_paths = {
	//BANK
	{
		glm::vec3(559.948730, 31.299992, -360.090973),
		glm::vec3(419.948730, 31.299992, -360.090973),
		glm::vec3(419.948730, 31.299992, -500.090973),
		glm::vec3(559.948730, 31.299992, -500.090973)
	},
	//SUBURBS
	{
		glm::vec3(420.010925, 31.455765, -280.130432),
		glm::vec3(740.010925, 31.455765, -280.130432),
		glm::vec3(739.948730, 21.455765, -60.534622),
		glm::vec3(419.948730, 21.455765, -60.534622)
	},
	//DOWNTOWN1
	{
		glm::vec3(100.000031, 0.299998, -220.079498),
		glm::vec3(220.000031, 0.299998, -220.079498),
		glm::vec3(220.000031, 0.299998, -120.079498),
		glm::vec3(100.000031, 0.299998, -120.079498)
	},
	//DOWNTOWN2
	{
		glm::vec3(-99.999969, 0.299998, -220.079498),
		glm::vec3(0.000031, 0.299998, -220.079498),
		glm::vec3(0.000031, 0.299998, -60.079498),
		glm::vec3(-99.999969, 0.299998, -60.079498)
	}
};





PoliceCar::PoliceCar(int ID) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(10.f, 0, 0)) {

	// Make Headlights
	float len = 20.f;
	float width = 5.f;
	auto pos = getPos() + glm::vec3(width, 0, len);
	physx::PxVec3 t_pos = physx::PxVec3(pos.x - width, pos.y, -pos.z + 3*pos.z/4);
	headlights = new BoxTrigger(false, t_pos, 8.f, 2.f, len);
	headlights->addJoint(actorPtr, startTransform);
}



void PoliceCar::createModel() {

	Model police_car_chassis(POLICE_CAR_CHASSIS_PATH);
	Model police_car_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_car_rwheel(POLICE_CAR_RWHEEL_PATH);

	car = new CarModel4W(police_car_chassis, police_car_lwheel, police_car_rwheel);
}


// Game logic to handle Per frame
void PoliceCar::handle(GLFWwindow* window, Player& player, State& state) {

	switch (ai_state) {
		case AISTATE::IDLE:
			idle(state.timeStep);
			break;
	
		case AISTATE::PATROL:
			patrol(window);
			break;
	
		case AISTATE::CHASE:
			chase(window, player, state.timeStep);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HANDLE AI STATES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Stop for MAX_IDLE_TIME seconds before continuing to patrol
void PoliceCar::idle(double timestep) {

	// Just started (might need to move this elsewhere
	if (idleTime <= 0) idleTime = MAX_IDLE_TIME;

	idleTime -= timestep; // TODO use Timer instead

	if (idleTime <= 0) ai_state = AISTATE::PATROL;
}



// Go forward until reached a node (intersection) and then randomly choose a direction
// If reached a dead end, back up and go back
void PoliceCar::patrol(GLFWwindow* window) {

	// if distance between cop car and its target is less than the threshold, switch to next target location
	if (glm::distance(getPos(), ai_paths[ID - 1][targetIndex]) < 10.f) {
		// set target index to the next location. if we're at the end of the location list, target the first location again
		targetIndex < ai_paths[ID - 1].size() - 1 ? targetIndex++ : targetIndex = 0;
	}

	// determine AI inputs
	glm::vec3 targetPos = ai_paths[ID - 1][targetIndex];
	updateSpeed(PATROL_ACCEL);
	driveTo(targetPos);
}


// AI only goes forward atm
// Resets after timer runs out
void PoliceCar::chase(GLFWwindow* window, Player& player, double timestep) {

	updateSpeed(CHASE_ACCEL);
	driveTo(player.getPos());

	if (player.isSeen) {
		startChase();	// Reset timer
	}
	else {
		chaseTime -= timestep; // TODO use timer instead
	}

	if (chaseTime <= 0) {
		//TODO stop siren
		std::cout << "CHASING END" << std::endl;
		ai_state = AISTATE::PATROL;
		reset();						//Teleport to start node
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GENERAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Initiate chase
void PoliceCar::startChase() {
	chaseTime = MAX_CHASE_TIME;
	ai_state = AISTATE::CHASE;
}



// Stunned
void PoliceCar::stun(double seconds) {
	if (seconds > 0) idleTime = seconds;
	ai_state = AISTATE::IDLE;
}




// Drive to given location
void PoliceCar::driveTo(glm::vec3 targetPos) {

	inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);
	glm::vec3 dir = targetPos - this->getPos();
	float temp = atan2(glm::dot(glm::cross(this->getDir(), dir), glm::vec3(0.f, 1.f, 0.f)), glm::dot(dir, this->getDir()));

	if (temp < 0) {
		if (temp < -0.05f) inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
	}
	else {
		if (temp > 0.05f) inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
	}
}