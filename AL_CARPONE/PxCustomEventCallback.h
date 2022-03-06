#pragma once
#include <iostream>
#include "State.h"
#include "Player.h"
#include "Bank.h"

using namespace physx;

//TODO CLEAN UP LATER
class PxCustomEventCallback : public PxSimulationEventCallback {

private:
	State& state; 
	Player& player; 
	Bank& bank;
	
public:

	PxCustomEventCallback(State& s, Player& p, Bank& b) : state(s), player(p), bank(b) {}

	// Shapes which have been marked as triggers using PxShapeFlag::eTRIGGER_SHAPE will send events according to the pair flag specification in the filter shader
	//This is a custom callback function to use when the player collides with pickups, or the trigger in front of the bank.
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {
	
		//starter code taken from https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
		for (PxU32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

		
			//If it is the player and the bank trigger interacting:
			if ((pairs[i].otherActor == player.actorPtr) &&		//bank.triggerPtr
				(pairs[i].triggerActor == bank.triggerPtr))
			{
				player.setRob(!player.canRob(state));
			}
			//////////////////
			//test triggar ptr
			/////////////////
			
			if ((pairs[i].otherActor == player.actorPtr) &&		//bank.triggerPtr
				(pairs[i].triggerActor == bank.testTriggerPtr))
			{
				//player.setRob(!player.canRob(state));
				player.setChooseTool(!player.canChooseTool(state));
			}
			
		}
	}
	
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) { PX_UNUSED((pairHeader)); PX_UNUSED((pairs)); PX_UNUSED((nbPairs)); }
};


