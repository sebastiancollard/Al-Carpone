#pragma once
#include <iostream>
#include "State.h"
#include "Player.h"
#include "PoliceCar.h"
#include "Building.h"
#include "PowerUp.h"
#include "AudioSystem.h"

using namespace physx;

static enum DEBUGMODE {
	FALSE,
	NOJAIL,
	NOALERT,
	NOCOPS
};

extern DEBUGMODE debugmode;

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
						/*cout << "px in trigger area " << endl;
						state.inTrigger = true;*/
						state.audioSystemPtr->playSoundEffect(SOUND_SELECTION::POP_TRIGGER_MENU);
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
					else 
					{
						
						
					};
					//if (pairs[i].triggerActor == b->CornetStoreTrigger1->ptr || pairs[i].triggerActor == b->CornetStoreTrigger2->ptr) //corner store trigger area
					//{		
					//	b->cornerRange = !b->cornerRange;
					//	
					//}
				}
				
			}
			
			// Headlights
			/*
			for (PoliceCar* popo : state.activePoliceVehicles) { // Iterate through policeCars
				if (pairs[i].triggerActor == popo->headlights->ptr) {
					//PLAYER VS HEADLIGHTS
					if ((pairs[i].otherActor == player.actorPtr) && (player.isDetectable()) && (debugmode == DEBUGMODE::FALSE || debugmode == DEBUGMODE::NOJAIL)) {
						popo->playerDetected = !popo->playerDetected;
						state.alertPolice();
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
			*/
		}
		/*cout << "px not in trigger area " << endl;
		state.inTrigger = false;*/
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

					for (PoliceCar* popo : state.activePoliceVehicles) { // Iterate through policeCars
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
				}
			}
		}
	}
	
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
};


