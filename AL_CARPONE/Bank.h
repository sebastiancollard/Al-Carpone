#pragma once

#include "glm/glm.hpp"
#include "PxPhysicsAPI.h"
#include "Building.h"
#include "State.h"
#include "Player.h"


#define BANK_BUILDING_PATH "models/al_carpone/bank_box.obj"




class Bank : public Building {


	physx::PxVec3 b_pos = physx::PxVec3(490, 30, -430);	// Position currently hardcoded

	float depth = 17.f;	 //y axis
	float height = 17.f; //z axis
	float width = 17.f;	 //x axis

public:

	unsigned int robRate = 5;


	Bank() { createTrigger(); }

	void createTrigger(); // Must only be called once!
	void triggerFunction(Player& player, State& state); // ROB BANK
};










/* UNUSED 

enum class ORIENT {N, E, S, W};	//N=1, E=2, S=3, W=4		North is positive z direction of global coordinates


class Bank : public Building {

private:
	//Bank position on map (tile position?)
	glm::vec3 origin = { 0.f, height/2.f, 0.f };		//arbitrary position for now. Note that the second param is it's vertical position. This should be height/2 for it to "touch the ground".

	ORIENT dir = ORIENT::S;		//Bank faces "south" by default



public:


	Bank() {
		//SETUP BANK POSITION, DIMENSIONS & ORIENTATION
		setPos(40.f, 0.f);
		setDir(ORIENT::E);
		setDimensions(17.f, 17.f, 17.f);
	}

	void createModel() {
		//model = new Model(BANK_BUILDING_PATH);
	}

	void createActors();

	glm::vec3 getPos() {
		return origin;
	}

	ORIENT getDir() {
		return dir;
	}

	glm::vec3 getDimsAsVec3() {
		return glm::vec3(width, depth, height);
	}

	float getDepth() {
		return depth;
	}
	
	float getWidth() {
		return width;
	}
	
	float getHeight() {
		return height;
	}

	// NOTE: setting position, size, and direction must be done *before* initPhysics() is called and the physics actors for the bank are created.
	// The following functions are more for completeness and may never be used (unless these attributes are going to be dynamically generated). 
	// Setting default position, dimensions and orientation can be done directly using the constants and default private variables above.
	
	void setPos(float x, float z) {
		//y (vertical) position is set as height/2 so that the building is always on the ground
		origin.x = x;
		origin.z = z;
	}
	
	void setDir(ORIENT direction) {
		dir = direction;
	}

	void setDimensions(float w, float d, float h) {
		width = w;
		depth = d;
		height = h;
		
		//y (vertical) position is set as height/2 so that the building is always on the ground
		origin.y = height / 2.f;
	}
};
*/