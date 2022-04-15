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
static enum DEBUGMODE {
	FALSE,
	TRUE,
	NOJAIL,
	NOALERT,
	NOCOPS,
};



PoliceCar::PoliceCar(int ID, DrivingNodes* drivingNodes) : Vehicle(VEHICLE_TYPE::POLICE_CAR, ID, physx::PxVec3(10.f, 0, 0)) {

	// Make Headlights
	//float len = 50.f;
	//float width = 10.f;
	//auto pos = getPos() + glm::vec3(width, 0, len);
	//physx::PxVec3 t_pos = physx::PxVec3(pos.x - width, pos.y, -pos.z + 3*pos.z/4);
	//headlights = new BoxTrigger(false, t_pos, 30.f, 2.f, len);
	//headlights->addJoint(actorPtr, startTransform);
	headlightForwardOffset = 1.698;
	headlightHorizontalOffset = 0.809;
	headlightVerticalOffset = -0.009;
	dNodes = drivingNodes;
	shouldReset = false;
	//myThread = std::thread(&updateLoop,this);
	//myThread = std::thread(&updateLoop);
}

void PoliceCar::update(Player& player, State& state) {

	extern DEBUGMODE debugmode;

	playerPos = player.getPos();

	myPos = getPos();
	myDir = getDir();
	myForwardSpeed = getForwardVelocity();

	if (shouldReset) reset();
	shouldReset = false;


	glm::vec3 direction = -glm::normalize(playerPos - myPos);

	PxVec3 vOffset(0, 2.0f, 0);

	PxVec3 origin = PxVec3(playerPos.x, playerPos.y, playerPos.z) + vOffset;
	PxVec3 unitDir = PxVec3(direction.x, direction.y, direction.z);
	PxRaycastBuffer hit;

	distanceToPlayer = glm::distance(playerPos, myPos);
	playerInTrigger = distanceToPlayer < detectionRadius;
	playerInJailRadius = distanceToPlayer < jailRadius;
	playerInSight = !gScene->raycast(origin, unitDir, distanceToPlayer, hit, PxHitFlag::eMESH_BOTH_SIDES);
	playerDetected = playerInSight && playerInTrigger && player.isDetectable();
	playerArrestable = playerInSight && playerInJailRadius && player.isDetectable();


	for (auto& p : state.active_items) {
		if((p.getType() == DONUT) && (p.itemInWorld)) {
			glm::vec3 donutPos = p.getPos();
			float distance = glm::distance(donutPos, myPos);

			direction = -glm::normalize(donutPos - myPos);

			origin = PxVec3(donutPos.x, donutPos.y, donutPos.z) + vOffset;
			PxVec3 unitDir = PxVec3(direction.x, direction.y, direction.z);
			bool seesDonut = !gScene->raycast(origin, unitDir, distance, hit, PxHitFlag::eMESH_BOTH_SIDES);

			if (distance < detectionRadius * 3 && seesDonut) {
				eatDonut(donutPos);
				return;
			}
			//break;
		}
	}
	

	if (debugmode == DEBUGMODE::NOALERT) return;

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

void PoliceCar::handleStuckPatrol() {

	if (x_z_distance_squared(dNodes->getNextPatrolNodePosition(ID, 0), playerPos) > 2500 && !playerInSight) {	//I doubled the distance: 1000 was still basically within the chase trigger region
		shouldReset = true;
		return;
	}

	int min_dist_index = 0;
	float min_dist = 9999999;
	for (int i = 0; i < dNodes->getPatrolRoutes()[ID].size(); i++) {
		glm::vec3 pos = dNodes->getNextPatrolNodePosition(ID, i);
		float dist = x_z_distance_squared(myPos, pos);
		if (dist < min_dist) {
			min_dist = dist;
			min_dist_index = i;
		}
	}
	targetIndex = min_dist_index;
	targetPosition = dNodes->getNextPatrolNodePosition(ID, min_dist_index);
}

void PoliceCar::reverse(double timestep, glm::vec3 playerPos) {
	inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_REVERSE);
	reverseTime -= timestep;

	if (reverseTime <= 0) {
		reverseTime = 0;
		if (ai_state == AISTATE::CHASE) targetPosition = dNodes->getClosestNodePosition(myPos); //Re-orient self
		else handleStuckPatrol();

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

	if (idleTime <= 0) {
		ai_state = AISTATE::PATROL;
		isStunned = false;
	}
}



// Go forward until reached a node (intersection) and then randomly choose a direction
// If reached a dead end, back up and go back
void PoliceCar::patrol() {

	if (chaseJustEnded || findingPatrolRoute) {

		glm::vec3 nextPatrolNode = dNodes->getNextPatrolNodePosition(ID, targetIndex);

		if (chaseJustEnded) targetPosition = dNodes->guideMeFromTo(myPos, nextPatrolNode);

		if (x_z_distance_squared(myPos, nextPatrolNode) < 100.0f) {
			targetPosition = nextPatrolNode;
			findingPatrolRoute = false;
		}

		else if (x_z_distance_squared(myPos,targetPosition) < 100.0f) {
			if (ID == 0) printf("hi\n");
			targetPosition = dNodes->guideMeFromTo(myPos, nextPatrolNode);
		}

		chaseJustEnded = false;
	}
	else {
		// if distance between cop car and its target is less than the threshold, switch to next target location
		if (x_z_distance_squared(myPos, dNodes->getNextPatrolNodePosition(ID, targetIndex)) < 100.f) {
			// set target index to the next location. if we're at the end of the location list, target the first location again
			dNodes->incrementTargetIndex(ID, targetIndex);// targetIndex < dNodes->getPatrolRoutes()[ID].size() ? targetIndex++ : targetIndex = 0;
		}

		// determine AI inputs
		targetPosition = dNodes->getNextPatrolNodePosition(ID, targetIndex);
	}

	
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
		
	}

	//If we need a hard turn
	if (glm::dot(myDir, glm::normalize(myPos - targetPosition)) < 0.5 && myForwardSpeed > 20.f) {
		brakeTime = 0.5f;
		return;
	}

	chaseTime -= timestep; // TODO use timer instead
	driveTo(targetPosition);
	
	if (chaseTime <= 0) {
		//TODO stop siren
		//std::cout << "CHASING END" << std::endl;
		ai_state = AISTATE::PATROL;
		chaseJustEnded = true;
		findingPatrolRoute = true;
		//shouldReset = true;					//Teleport to start node
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


void PoliceCar::eatDonut(glm::vec3 donutPos) {

	if (glm::distance(donutPos, myPos) < 3.0f)	//using jailRadius as radius where police "stops" to eat donut
	{
		stun(1);	//just stun for 1 second. This will keep being called until the item despawns, so that the stun actually lasts as long as the items remaining timer
	}
	else {
		driveTo(donutPos);
	}
}

std::pair<glm::vec3, glm::vec3> PoliceCar::getHeadlightPositions(State*  state) {
	glm::vec3 origin = getPos();
	glm::vec3 uDir = getDir();
	glm::vec3 uRight = getRight();
	glm::vec3 uUp = getUp();

	float dForward = headlightForwardOffset;
	float dRight = headlightHorizontalOffset;
	float dUp = headlightVerticalOffset;

	glm::vec3 frontCenter = origin + uUp * dUp + uDir * dForward;
	glm::vec3 rightDiff = uRight * dRight;

	return std::pair<glm::vec3, glm::vec3>(glm::vec3(frontCenter - rightDiff), glm::vec3(frontCenter + rightDiff));
}