//We can split up the classes in this file into files with better names

#include<iostream>
#include<string>

#define MAIN_MENU_GAME_STATE 1000001
#define PLAY_GAME_STATE 1000002
#define PAUSE_GAME_STATE 1000003
#define GARAGE_GAME_STATE 1000004


class State {
private:
	int fpsCounter = 0;
	char* fps_c;

public:
	double timestep;

	char* getFPS() {
		fpsCounter--;
		if (fpsCounter <= 0) {
			std::string fps = std::to_string((int)((double)CLOCKS_PER_SEC / timestep));

			delete[] fps_c;
			fps_c = new char[fps.length() + 1];
			strcpy_s(fps_c, fps.length() + 1, fps.c_str());

			fpsCounter = 10;
		}
		return fps_c;
	}

};

class Car{
    public:
        float mass;
        //glm::vec3 position;
        //glm::vec3 velocity;
        //glm::vec3 acceleration;
        //glm::vec3 direction;

        //Texture texture;
        //Model model;
};

class Player : public Car {
public:
    //std::vector<Powerup*> powerups;
    //std::vector<Upgrade*> upgrades;
};

class Enemy : public Car {
};

class Powerup {
    double timer;
};

class Upgrade {
    int price;
};