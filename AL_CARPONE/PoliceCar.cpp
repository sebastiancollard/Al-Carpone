#include "PoliceCar.h"
#include "Player.h"

#define POLICE_CAR_CHASSIS_PATH "models/police_car/police_car_chassis.obj"
#define POLICE_CAR_LWHEEL_PATH "models/police_car/police_car_wheel_left.obj"
#define POLICE_CAR_RWHEEL_PATH "models/police_car/police_car_wheel_right.obj"

#define MAX_IDLE_TIME 20 //20 seconds?




PoliceCar::PoliceCar(int ID) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(10.f, 0, 0)) {

	// Make Headlights
	float len = 10.f;
	auto pos = getPos() + glm::vec3(0, 0, len);
	physx::PxVec3 t_pos = physx::PxVec3(pos.x, pos.y, -pos.z);
	headlights = new BoxTrigger(false, 5.f, 4.f, len, t_pos); 
	headlights->addJoint(actorPtr, startTransform);
}



void PoliceCar::createModel() {

	Model police_car_chassis(POLICE_CAR_CHASSIS_PATH);
	Model police_car_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_car_rwheel(POLICE_CAR_RWHEEL_PATH);

	car = new CarModel4W(police_car_chassis, police_car_lwheel, police_car_rwheel);
}

void PoliceCar::handle(GLFWwindow* window, glm::vec3 targetPos) {

	switch (state) {
		case AISTATE::IDLE:
			break;
	
		case AISTATE::PATROL:
			patrol(window, targetPos);
			break;
	
		case AISTATE::CHASE:
			chase(window, targetPos);
			break;
	}

	// Move Headlights with car
	auto pos = getPos();
	physx::PxVec3 t_pos = physx::PxVec3(pos.x+10.f, 0, pos.z);
	headlights->setPos(t_pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HANDLE AI STATES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Stop for MAX_IDLE_TIME seconds before continuing to patrol
void PoliceCar::idle() {

	// Just started (might need to move this elsewhere
	if (idleTime == 0) idleTime = MAX_IDLE_TIME;

	idleTime--; // TODO change to seconds instead

	if (idleTime == 0) state = AISTATE::PATROL;
}



// Go forward until reached a node (intersection) and then randomly choose a direction
// If reached a dead end, back up and go back
void PoliceCar::patrol(GLFWwindow* window, glm::vec3 targetPos) {
	inputQueue.push(DriveMode::eDRIVE_MODE_PATROL_FORWARDS);
	glm::vec3 dir = targetPos - this->getPos();
	float temp = atan2(glm::dot(glm::cross(this->getDir(), dir), glm::vec3(0.f, 1.f, 0.f)), glm::dot(dir, this->getDir()));
	if (temp < 0) {
		//std::cout << "right" << std::endl;
		if (temp < -0.05f)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
	}
	else {
		//std::cout << "left" << std::endl;
		if (temp > 0.05f)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
	}
}


// AI only goes forward atm
// TODO: respawn if stuck
// TODO: follow player's last pos before an intersection instead
void PoliceCar::chase(GLFWwindow* window, glm::vec3 playerPos) {

	inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);
	glm::vec3 dir = playerPos - this->getPos();
	float temp = atan2(glm::dot(glm::cross(this->getDir(), dir), glm::vec3(0.f, 1.f, 0.f)), glm::dot(dir, this->getDir()));
	if (temp < 0) {
		//std::cout << "right" << std::endl;
		if (temp < -0.05f)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
	}
	else {
		//std::cout << "left" << std::endl;
		if (temp > 0.05f)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
	}
}