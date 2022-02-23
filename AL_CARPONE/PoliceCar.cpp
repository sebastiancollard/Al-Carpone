#include "PoliceCar.h"
#include "Player.h"

#define POLICE_CAR_CHASSIS_PATH "models/police_car/police_car_chassis.obj"
#define POLICE_CAR_LWHEEL_PATH "models/police_car/police_car_wheel_left.obj"
#define POLICE_CAR_RWHEEL_PATH "models/police_car/police_car_wheel_right.obj"

void PoliceCar::createModel() {

	Model police_car_chassis(POLICE_CAR_CHASSIS_PATH);
	Model police_car_lwheel(POLICE_CAR_LWHEEL_PATH);
	Model police_car_rwheel(POLICE_CAR_RWHEEL_PATH);

	car = new CarModel4W(police_car_chassis, police_car_lwheel, police_car_rwheel);
}

void PoliceCar::handle(GLFWwindow* window, glm::vec3 playerPos) {
	inputQueue.push(DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);
	glm::vec3 dir = playerPos - this->getPos();
	float temp = atan2(glm::dot(glm::cross(this->getDir(), dir), glm::vec3(0.f, 1.f, 0.f)), glm::dot(dir, this->getDir()));
	if (temp < 0) {
		//std::cout << "right" << std::endl;
		if (temp < -0.1f)
		inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);
	}
	else {
		//std::cout << "left" << std::endl;
		if (temp > 0.1f)
			inputQueue.push(DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);
	}

}