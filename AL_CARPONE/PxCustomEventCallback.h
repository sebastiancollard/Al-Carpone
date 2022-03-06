#pragma once
#include <iostream>
#include "State.h"
#include "Player.h"
#include "PoliceCar.h"
#include "Building.h"

using namespace physx;

//TODO CLEAN UP LATER
class PxCustomEventCallback : public PxSimulationEventCallback {

private:
	State& state; 
	Player& player; 
	
public:

	PxCustomEventCallback(State& s, Player& p) : state(s), player(p) {}

	// Shapes which have been marked as triggers using PxShapeFlag::eTRIGGER_SHAPE will send events according to the pair flag specification in the filter shader
	//This is a custom callback function to use when the player collides with pickups, or the trigger in front of the bank.
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {
	

		std::cout << "TRIGGER ACTIVATED" << std::endl;

		//starter code taken from https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
		for (PxU32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

		
			// Player's interactions
			if (pairs[i].otherActor == player.actorPtr)
			{
				// Buildings (bank, cornerstores, etc)
				for (Building* b : state.buildings) {
					if (pairs[i].triggerActor == b->trigger->ptr) {
						b->isInRange = !b->isInRange; // Set boolean value
						std::cout << "BUILDING IN RANGE!!!" << std::endl;
					}
				}

				// Headlights
				for (Vehicle* v : state.activeVehicles) { // Iterate through policeCars

					if (v == state.activeVehicles[0]) { // This is player -> skip
						continue;
					}

					else {
						PoliceCar* popo;

						try {
							popo = (PoliceCar*)v; // Just in case
						}
						catch (exception e) {
							std::cerr << "Couldn't cast vehicle to PoliceCar: " << e.what() << std::endl;
							continue;
						}
						
						if (pairs[i].triggerActor == popo->headlights->ptr) {
							std::cout << "HEALIGHTS!!!" << std::endl;
							popo->state = AISTATE::CHASE;
						}
					}
				}
			}
		}
	}
	
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) { PX_UNUSED((pairHeader)); PX_UNUSED((pairs)); PX_UNUSED((nbPairs)); }
};

