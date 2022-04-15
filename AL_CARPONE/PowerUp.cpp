#include "PowerUp.h"

//Constructors
PowerUp::PowerUp() {}

PowerUp::PowerUp(const PowerUp &p) {
	type = p.type;
	model_path = p.model_path;			

	timer = p.timer;				//a little worried about this one (will they share a instance of this timer??) Should use timer copy constructor
	duration_sec = p.duration_sec;		
									
	power_used = p.power_used;		
	timer_active = p.timer_active;		
	throw_item = p.throw_item;
	drop_item = p.drop_item;
	itemInWorld = p.itemInWorld;
	actorPtr = p.actorPtr;
}

PowerUp::PowerUp(POWER_TYPE power_type) {
	type = power_type;
	duration_sec = 15.f;					//default is 15 seconds (choose 0 duration for there to be no timer)
}


PowerUp::PowerUp(POWER_TYPE power_type, float duration) {
	type = power_type;
	duration_sec = duration;
	timer.reset();
}


//public functions
void PowerUp::reset() {
	setType(NONE);
	actorPtr = NULL;
	itemInWorld = false;
	power_used = false;	
	timer_active = false;		
	throw_item = false;
	drop_item = false;
}

void PowerUp::activateTimed() {
	if (!timer_active) {
		timer_active = true;
		power_used = true;
		timer.reset();
	}
}

void PowerUp::deactivateTimed() {
	timer_active = false;
	//leaving as separate function from update in case more is added here
}

void PowerUp::updateTimed() {
	if ((duration_sec > 0.f) && (timer_active)) {
		timer.tick();
		if(timer.getDeltaTime() > duration_sec) {
			deactivateTimed();
		}
	}
}
	
//This should be triggered when the player pressed some "use ability" button for the throwable/placeable items.
void PowerUp::dropOrThrow() {
	if (!power_used) {
		switch (type) {
		case TOMATO:
			model_path = TOMATO_PATH;
			throw_item = true;
			activateTimed();
			break;
		case DONUT:
			//player to throws donut
			model_path = DONUT_PATH;
			throw_item = true;
			activateTimed();
			break;
		case SPIKE_TRAP:
			model_path = SPIKE_PATH;
			drop_item = true;
			activateTimed();
			break;
		default:
			//Do nothing
			break;
		}
	}
}

void PowerUp::stopThrow() {
	throw_item = false;
}

void PowerUp::stopDrop() {
	drop_item = false;
}

POWER_TYPE PowerUp::getType() {
	return type;
}

void PowerUp::setType(POWER_TYPE t) {
	type = t;
	
	duration_sec = 0.f;
	
	power_used = false;
	throw_item = false;
	drop_item = false;
	timer_active = false;
}

void PowerUp::setDuration(float d) {
	duration_sec = d;
}

std::string PowerUp::getModelPath() {
	return model_path;
}

//should the item be "despawned" from the world (after it has been thrown, and the countdown timer has run out)
bool PowerUp::shouldDespawn() {
	return (!timer_active && power_used && (duration_sec > 0.f));
}

//bool PowerUp::shouldDespawn() {
	//return (!timer_active && power_used && (duration_sec <= 0.f));
//}

bool PowerUp::isActive() {
	return (timer_active && power_used);
}

glm::vec3 PowerUp::getPos() {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	const PxU32 nbShapes = actorPtr->getNbShapes();
	actorPtr->getShapes(shapes, nbShapes);

	PxTransform shapePose(PxShapeExt::getGlobalPose(*shapes[0], *actorPtr));
	return glm::vec3(shapePose.p.x, shapePose.p.y, shapePose.p.z);
}

float PowerUp::getRemainingTime() {
	return (duration_sec - timer.getDeltaTime());
}