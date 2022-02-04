#pragma once
#include <iostream>

class PxCustomEventCallback : public physx::PxSimulationEventCallback {
	
public:
	// Shapes which have been marked as triggers using PxShapeFlag::eTRIGGER_SHAPE will send events according to the pair flag specification in the filter shader
	//This is a custom callback function to use when the player collides with pickups, or the trigger in front of the bank.
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {
		std::cout << "onTrigger called" << std::endl;

		//starter code taken from https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
		for (PxU32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			if ((pairs[i].otherShape->getActor() == player.actorPtr) &&
				(pairs[i].triggerShape->getActor() == bank.triggerPtr))
			{
				std::cout << "collision with trigger!" << std::endl;
				player.can_rob = true;
			}
		}
	}
	
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }

	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
		std::cout << "onContact called" << std::endl;
		PX_UNUSED((pairHeader));
		PX_UNUSED((pairs));
		PX_UNUSED((nbPairs));
	}
};
