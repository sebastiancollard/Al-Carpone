#pragma once

//These aren't really needed as constants but will keep them here for easy modification for now
#define WIDTH	5.f
#define DEPTH	5.f
#define HEIGHT	10.f

enum orient {N, E, S, W};	//N=1, E=2, S=3, W=4

class Bank {
private:
	//Bank position on map (tile position?)
	glm::vec3 pos = { 40.f, HEIGHT/2.f + 3.5f, 20.f };		//arbitrary position for now. Note that the second param is it's vertical position. This should be height/2 for it to "touch the ground".

	orient dir = N;		//Bank faces "north" by default

	float depth = DEPTH;	//y axis
	float height = HEIGHT;	//z axis
	float width = WIDTH;	//x axis

public:
	//physX bank actor and trigger actor
	PxRigidActor* bankPtr;				//Would like to make these actors private, however they are not provate for player. Thought it would be best to keep it the same to avoid confusion.
	PxRigidActor* triggerPtr;


	glm::vec3 getPos() {
		return pos;
	}

	orient getDir() {
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

	/*NOTE: setting position, size, and direction must be done *before* initPhysics() is called and the physics actors for the bank are created.
	* The following functions are more for completeness and may never be used (unless these attributes are going to be dynamically generated). 
	* Setting default position, dimensions and orientation can be done directly using the constants and default private variables above.
	* */
	void setPos(float x, float z) {
		//y (vertical) position is set as height/2 so that the building is always on the ground
		pos.x = x;
		pos.z = z;
	}
	
	void setDir(orient direction) {
		dir = direction;
	}

	void setDimensions(float w, float d, float h) {
		width = w;
		depth = d;
		height = h;
		
		//y (vertical) position is set as height/2 so that the building is always on the ground
		pos.y = h / 2.f;
	}


};