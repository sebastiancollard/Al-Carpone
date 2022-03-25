#pragma once
#include <iostream>
#include "State.h"
#include "Player.h"
#include "PoliceCar.h"
#include "Building.h"
#include "PowerUp.h"

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
	

		//std::cout << "TRIGGER ACTIVATED" << std::endl;

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
				for (int j = 0; j < state.buildings.size(); j++) {
					if (state.buildings[j] == nullptr) continue;
					if (pairs[i].triggerActor == state.buildings[j]->trigger->ptr) {
						state.buildings[j]->isInRange = !state.buildings[j]->isInRange; // Set boolean value
						if (state.buildings[j]->isInRange) {
							//std::cout << "BUILDING IN RANGE!!!" << std::endl;
							if (j == 1 || j == 2 || j == 3) {
								garageDoorOpen = true;
								for (PoliceCar* p : state.activePoliceVehicles) {
									if (p->chaseTime > 0) garageDoorOpen = false;
									break;
								}
							}
						
						}
						else {
							//std::cout << "BUILDING OUT OF RANGE!!!" << std::endl;
							if (j == 1 || j == 2 || j == 3); // garageDoorOpen = false;
						} 
					}
					//if (pairs[i].triggerActor == b->CornetStoreTrigger1->ptr || pairs[i].triggerActor == b->CornetStoreTrigger2->ptr) //corner store trigger area
					//{		
					//	b->cornerRange = !b->cornerRange;
					//	
					//}
				}
			}

			// Headlights
			/*for (Vehicle* v : state.activeVehicles) { // Iterate through policeCars

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
						//PLAYER VS HEADLIGHTS
						if ((pairs[i].otherActor == player.actorPtr) && (player.isDetectable())) {
							player.isSeen = !player.isSeen;
							//std::cout << player.isSeen << std::endl;
							if (player.isSeen) popo->startChase();
								player.jailTimer.reset();
						}
						
						//DONUT VS HEADLIGHTS 
						else if ((player.getPower()->getType() == DONUT) && (pairs[i].otherActor == player.getPower()->actorPtr)) {
							if (popo->isStunned) {
								popo->isStunned = false;
							}
							else {
								popo->stun(5);				//Default stun for 5 seconds. Adjust? could stun police until item despawns.
								std::cout << "Police spotted a donut!" << std::endl;
							}
						}
					}
				}
			}*/
		}
	}

	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) 
	{ 
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];
			
			physx::PxRigidActor* itemActr = player.getPower()->actorPtr;
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if ((pairHeader.actors[0] == itemActr) || (pairHeader.actors[1] == itemActr))
				{
					PxActor* otherActor = (itemActr == pairHeader.actors[0]) ? pairHeader.actors[1] : pairHeader.actors[0];
					PoliceCar* popo;

					/*for (Vehicle* v : state.activeVehicles) { // Iterate through policeCars

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
							//TOMATO collision with police chassis or wheel
							if ((player.getPower()->getType() == TOMATO) && (otherActor == popo->actorPtr)) {
								if (popo->isStunned) {
									popo->isStunned = false;
								}
								else {
									popo->stun(5);				//Default stun for 5 seconds. Adjust? could stun police until item despawns.
									std::cout << "Police was hit by a tomato!" << std::endl;
								}
							}
							//SPIKE TRAP collision with police chassis or wheel
							else if ((player.getPower()->getType() == SPIKE_TRAP) && (otherActor == popo->actorPtr)) {
								if (popo->isStunned) {
									popo->isStunned = false;
								}
								else {
									popo->stun(5);				//Default stun for 5 seconds. Adjust? could stun police until item despawns.
									std::cout << "Police ran over a spike trap!" << std::endl;
								}
							}
						}
					}*/
				}
			}
		}
	}
	
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
};


