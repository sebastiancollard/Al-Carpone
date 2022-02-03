#pragma once

//These aren't really needed as constants but will keep them here for easy modification for now
#define WIDTH	5.f
#define DEPTH	5.f
#define HEIGHT	10.f

enum orient {N, E, S, W};	//N=1, E=2, S=3, W=4

class Bank {
private:
	//Bank position on map (tile position?)
	glm::vec3 pos = { 20.f, 10.f, 20.f };		//arbitrary position for now. Note that the second param is it's vertical position. This should be height/2 for it to "touch the ground".
												//height/2 is hard-coded into the initialization of the bank's physX actor. 

	//Bank orientation (N,E,S,W) -> determines which side the player can rob from/where the trigger is located
	orient dir = S;		//Bank faces "south" by default

public:
	//physX bank actor and trigger actor
	PxRigidActor* bankPtr;
	PxRigidActor* triggerPtr;
	float depth = DEPTH;	//y axis
	float height = HEIGHT;	//z axis
	float width = WIDTH;	//x axis

	glm::vec3 getPos() {
		return pos;
	}

	orient getDir() {
		return dir;
	}

	glm::vec3 getdDimsAsVec3() {
		return glm::vec3(width, depth, height);
	}
};