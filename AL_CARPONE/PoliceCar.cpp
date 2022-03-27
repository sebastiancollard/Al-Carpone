#include "PoliceCar.h"
#include "util.h"

#define POLICE_CAR_CHASSIS_PATH "models/police_car/police_car_chassis.obj"
#define POLICE_CAR_LWHEEL_PATH "models/police_car/police_car_wheel_left.obj"
#define POLICE_CAR_RWHEEL_PATH "models/police_car/police_car_wheel_right.obj"

// TODO not yet in seconds
#define MAX_IDLE_TIME 20		


// Car Speeds
#define PATROL_ACCEL 0.625f
#define PERSUIT_ACCEL 0.7f
#define CHASE_ACCEL 0.8f


PoliceCar::PoliceCar(int ID, DrivingNodes* drivingNodes) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(10.f, 0, 0)) {

	// Make Headlights
	float len = 50.f;
	float width = 10.f;
	auto pos = getPos() + glm::vec3(width, 0, len);
	physx::PxVec3 t_pos = physx::PxVec3(pos.x - width, pos.y, -pos.z + 3*pos.z/4);
	//headlights = new BoxTrigger(false, t_pos, 30.f, 2.f, len);
	//headlights->addJoint(actorPtr, startTransform);
	dNodes = drivingNodes;
	shouldReset = false;
	//myThread = std::thread(&updateLoop,this);
	//myThread = std::thread(&updateLoop);
}

void PoliceCar::update(glm::vec3 playerPos,State& state) {

	myPos = getPos();
	myDir = getDir();
	myForwardSpeed = getForwardVelocity();

	if (shouldReset) reset();
	shouldReset = false;


	glm::vec3 direction = -glm::normalize(playerPos - myPos);

	PxVec3 vOffset(0, 5.0f, 0);

	PxVec3 origin = PxVec3(playerPos.x, playerPos.y, playerPos.z) + vOffset;
	PxVec3 unitDir = PxVec3(direction.x, direction.y, direction.z);
	PxRaycastBuffer hit;

	distanceToPlayer = glm::distance(playerPos, myPos);
	playerInTrigger = distanceToPlayer < detectionRadius;
	playerInJailRadius = distanceToPlayer < jailRadius;
	playerInSight = !gScene->raycast(origin, unitDir, distanceToPlayer, hit, PxHitFlag::eMESH_BOTH_SIDES);
	playerDetected = playerInSight && playerInTrigger && state.playerPtr->isDetectable();
	playerArrestable = playerInSight && playerInJailRadius;

	if (playerDetected) state.alertPolice();

	//updateRequest = true;
	handle(playerPos,state.timeStep);
}


// Game logic to handle Per frame
void PoliceCar::handle(glm::vec3 playerPos, double timeStep) {

	if (playerDetected && ai_state != AISTATE::IDLE) startChase();

	if (vehicleInAir) {
		airTime += timeStep;
		if (airTime > 3.0f) {
			shouldReset = true;
			airTime = 0;
			return;
		}
	}

	if (brakeTime > 0) {
		brake(timeStep);
		return;
	}

	if (abs(myForwardSpeed) < 1.f && ai_state != AISTATE::IDLE) {
		stuckTime += timeStep;
		if (stuckTime > 1.5f) {
			if (getUp().y < 0) reset();
			reverseTime = 1.0f;
		}
	}
	else stuckTime = 0;
	
	if (reverseTime > 0) {
		reverse(timeStep,playerPos);
		return;
	}

	switch (ai_state) {
		case AISTATE::IDLE:
			idle(timeStep);
			break;
	
		case AISTATE::PATROL:
			patrol();
			break;
	
		case AISTATE::CHASE:
			chase(playerPos,timeStep);
			break;
	}

}


void PoliceCar::createModel() {

	Model police_car_chassis(POLICE_CAR_CHASSIS_PATH);
	Model police_car_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_car_rwheel(POLICE_CAR_RWHEEL_PATH);

	car = new CarModel4W(police_car_chassis, police_car_lwheel, police_car_rwheel);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HANDLE AI STATES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PoliceCar::reverse(double timestep, glm::vec3 playerPos) {
	inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);
	reverseTime -= timestep;

	if (reverseTime <= 0) {
		reverseTime = 0;
		targetPosition = dNodes->getClosestNodePosition(myPos); //Re-orient self

		if (playerInSight) {
			targetPosition = playerPos;
			targetingPlayer = true;
		}

	}
}


void PoliceCar::brake(double timestep) {
	inputQueue.push(DriveMode::eDRIVE_MODE_HANDBRAKE);
	brakeTime -= timestep;

	if (brakeTime <= 0) {
		brakeTime = 0;
		return;
	}

	glm::vec3 dir = targetPosition - myPos;
	float temp = atan2(glm::dot(glm::cross(myDir, dir), glm::vec3(0.f, 1.f, 0.f)), glm::dot(dir, myDir));

	if (temp < 0) {
		if (temp < -0.05f) inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
	}
	else {
		if (temp > 0.05f) inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
	}
}

// Stop for MAX_IDLE_TIME seconds before continuing to patrol
void PoliceCar::idle(double timestep) {

	// Just started (might need to move this elsewhere
	if (idleTime <= 0) idleTime = MAX_IDLE_TIME;

	idleTime -= timestep; // TODO use Timer instead

	if (idleTime <= 0) ai_state = AISTATE::PATROL;
	//isStunned = false;
}



// Go forward until reached a node (intersection) and then randomly choose a direction
// If reached a dead end, back up and go back
void PoliceCar::patrol() {

	// if distance between cop car and its target is less than the threshold, switch to next target location
	if (x_z_distance_squared(myPos, dNodes->getNextPatrolNodePosition(ID,targetIndex)) < 10.f) {
		// set target index to the next location. if we're at the end of the location list, target the first location again
		dNodes->incrementTargetIndex(ID, targetIndex);// targetIndex < dNodes->getPatrolRoutes()[ID].size() ? targetIndex++ : targetIndex = 0;
	}

	// determine AI inputs
	targetPosition = dNodes->getNextPatrolNodePosition(ID, targetIndex);
	updateSpeed(PATROL_ACCEL);
	driveTo(targetPosition);
}


// AI only goes forward atm
// Resets after timer runs out
void PoliceCar::chase(glm::vec3 playerPos,double timestep) {
	updateSpeed(CHASE_ACCEL);

	if (playerInSight) {
		targetPosition = playerPos;
		targetingPlayer = true;
	}

	else {
		if(targetingPlayer || x_z_distance_squared(myPos, targetPosition) < 900.0f){
			targetPosition = dNodes->guideMeFromTo(myPos, playerPos);
			targetingPlayer = false;
		} 
		chaseTime -= timestep; // TODO use timer instead
	}

	//If we need a hard turn
	if (glm::dot(myDir, glm::normalize(myPos - targetPosition)) < 0.5 && myForwardSpeed > 20.f) {
		brakeTime = 0.5f;
		return;
	}

	driveTo(targetPosition);
	
	if (chaseTime <= 0) {
		//TODO stop siren
		//std::cout << "CHASING END" << std::endl;
		ai_state = AISTATE::PATROL;
		shouldReset = true;					//Teleport to start node
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GENERAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//reset position and state
void PoliceCar::hardReset() {
	reset();
	shouldReset = true;
	ai_state = AISTATE::PATROL;
	chaseTime = 0;
}

// Initiate chase
void PoliceCar::startChase() {
	chaseTime = maxChaseTime;
	ai_state = AISTATE::CHASE;
}

// Stunned
void PoliceCar::stun(double seconds) {
	if (seconds > 0) idleTime = seconds;
	ai_state = AISTATE::IDLE;
	isStunned = true;
}

void PoliceCar::reset() {
	Vehicle::reset();
	reverseTime = 0;
	stuckTime = 0;
	targetIndex = 0;
	targetingPlayer = false;
	targetPosition = getPos();
}

// Drive to given location
void PoliceCar::driveTo(glm::vec3 targetPos) {
	//TODO if donut is closer than target position, then set target position to donut

	inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);
	glm::vec3 dir = targetPos - myPos;
	float temp = atan2(glm::dot(glm::cross(myDir, dir), glm::vec3(0.f, 1.f, 0.f)), glm::dot(dir, myDir));

	if (temp < 0) {
		if (temp < -0.05f) inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
	}
	else {
		if (temp > 0.05f) inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
	}
}