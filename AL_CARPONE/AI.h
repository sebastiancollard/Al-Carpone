#pragma once


void spawnPolice(State& state) {
	while (state.inactivePoliceVehicles.size() > 0) {
		state.activePoliceVehicles.push_back(state.inactivePoliceVehicles.back());
		state.inactivePoliceVehicles.pop_back();

		state.activePoliceVehicles.back()->reset();
	}
}

void despawnPolice(State& state) {
	while (state.activePoliceVehicles.size() > 0) {
		state.inactivePoliceVehicles.push_back(state.activePoliceVehicles.back());
		state.activePoliceVehicles.pop_back();
	}
}


void createPolice(DrivingNodes* drivingNodes, CarModel4W* policeCarModel, State& state) {
	for (int i = 0; i < (int)drivingNodes->getPatrolRoutes().size(); i++) {
		PoliceCar* p = new PoliceCar(i, drivingNodes);

		glm::vec3 glmPOS = drivingNodes->getNextPatrolNodePosition(i, 0);
		glm::vec3 glmDIR = drivingNodes->getNextPatrolNodePosition(i, 1) - drivingNodes->getNextPatrolNodePosition(i, 0);

		p->setStart(PxVec3(glmPOS.x, glmPOS.y, glmPOS.z), PxVec3(glmDIR.x, 0, glmDIR.z));

		p->reset();

		state.inactivePoliceVehicles.push_back(p);
	}
}

void deletePolice(State& state) {
	despawnPolice(state);
	for (PoliceCar* p : state.inactivePoliceVehicles) {
		delete(p);
	}
}


