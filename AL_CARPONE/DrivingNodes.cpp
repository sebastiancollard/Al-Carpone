#include"DrivingNodes.h"
#include"util.h"
#include <limits>





DrivingNodes::DrivingNodes(std::vector<glm::vec3> patrolPositions, std::vector<std::vector<unsigned int> > patrolConnections,
	std::vector<glm::vec3> navigationPositions, std::vector<std::vector<unsigned int> > navigationConnections) {

	load_patrol_routes(patrolPositions, patrolConnections);
	load_nodemap(navigationPositions, navigationConnections);
	doTheDijkstra();

	//printShortestRoutes();
	//printNodeMap();
}



//Returns the position of the node closest to the input position
unsigned int DrivingNodes::getClosestNodeIndex(glm::vec3 position) {
	unsigned int closest_node_index = 0;

	for (int i = 0; i < nodePositions.size(); i++) {
		glm::vec3 node = nodePositions[i];
		//ignore height in comparison
		if (x_z_distance_squared(node, position) < x_z_distance_squared(nodePositions[closest_node_index], position)) closest_node_index = i;
	}

	return closest_node_index;
}

glm::vec3 DrivingNodes::getClosestNodePosition(glm::vec3 position) {
	return nodePositions[getClosestNodeIndex(position)];
}


//Returns the next node in the input route that a police car at the input position should go to.
glm::vec3 DrivingNodes::getNextPatrolNodePosition(unsigned int routeNumber, unsigned int routeSequenceStep) {
	return patrol_routes[routeNumber][routeSequenceStep];
}



void DrivingNodes::printPatrolRoutes() {
	for (int i = 0; i < patrol_routes.size(); i++) {
		printf("ROUTE[%d]:\n", i);
		for (glm::vec3 pos : patrol_routes[i]) {
			printf("	<%.2f,%.2f,%.2f>\n", pos.x, pos.y, pos.z);
		}
	}
}

void DrivingNodes::printNodeMap() {
	for (std::pair<unsigned int, std::vector<unsigned int> > keypair : nodemap) {
		printf("%d : ", keypair.first);

		for (unsigned int j : keypair.second) {
			printf("%d ", j);
		}

		printf("\n");
	}
}

void DrivingNodes::incrementTargetIndex(unsigned int routeId, unsigned int& targetIndex) {
	unsigned int maxIndex = (unsigned int)patrol_routes[routeId].size() - 1;

	targetIndex++;

	if (targetIndex > maxIndex) targetIndex = 0;
}

void DrivingNodes::printShortestRoutes() {

	for (int nodeA = 0; nodeA < prev_node.size(); nodeA++) {
		for (int nodeB = 0; nodeB < prev_node.size(); nodeB++) {
			printf("LAST NODE IN PATH FROM [%d] TO [%d] : [%d]\n", nodeA, nodeB, prev_node[nodeA][nodeB]);
		}
	}
}


//Recursively travels through the route until only repeats exist in the connections.
//This will get every node in the route. Stores in third param.
void getFullPath(unsigned int node, std::vector<std::vector<unsigned int>> connections, std::vector<unsigned int>& route) {

	if (is_in(node, route)) return;

	else route.push_back(node);

	for (unsigned int connected_node : connections[node]) {
		getFullPath(connected_node, connections, route);
	}
}

//Loads all of the patrol routes from a list of every node and a list of all their connectivity. The input positions should
//only include patrol routes.
//This assumes that the direction of travel is expressed in the Z coordinate of the positions. The route should start at the lowest Z position
//and then go to the next in height and so on until the end, then it should loop.
void DrivingNodes::load_patrol_routes(std::vector<glm::vec3> positions, std::vector< std::vector<unsigned int> > connections) {

	std::vector< std::vector<unsigned int> > unordered_routes;
	//Get the full path of connectivity from each starting node.
	//This will get lots of repeats of the same loops. We need to clean this.
	for (int start_node = 0; start_node < connections.size(); start_node++) {
		std::vector<unsigned int> route;

		getFullPath(start_node, connections, route);

		unordered_routes.push_back(route);
	}


	//Remove all the repeats
	for (int i = 0; i < unordered_routes.size(); i++) {
		std::vector<unsigned int> current_route = unordered_routes[i];
		for (int j = 0; j < unordered_routes.size(); j++) {
			if (i == j) continue;

			//We are checking if the first element of the route at index j is in the current route. This
			//would imply that the routes are equal, because each route contains every possible node reachable
			//in the route. We can remove any entry that satifys this.
			if (is_in(unordered_routes[j][0], current_route)) {
				unordered_routes.erase(unordered_routes.begin() + j);
				//Size has changed so need to reset.
				j = 0;
			}


		}
	}


	std::vector< std::vector<unsigned int> > ordered_routes;
	//Order all routes based on their elements height values.
	for (std::vector<unsigned int>& route : unordered_routes) {

		std::vector<unsigned int> ordered_route;

		while (route.size() > 0) {
			int lowest = 0;
			//Find index in list next lowest height value
			for (int i = 0; i < route.size(); i++) if (positions[route[i]].y < positions[route[lowest]].y) lowest = i;
			//Add this index to the ordered set and remove from unordered as to not reselect it.
			ordered_route.push_back(route[lowest]);
			route.erase(route.begin() + lowest);
		}

		ordered_routes.push_back(ordered_route);

	}

	//Convert all ordered route indices to their respective positions and store in patrol_routes
	for (std::vector< unsigned int > route : ordered_routes) {

		std::vector<glm::vec3> position_route;
		for (unsigned int index : route) {

			position_route.push_back(positions[index]);
		}

		patrol_routes.push_back(position_route);
	}

}

//Loads a map from every node position and their connections. This should include both patrol nodes and non-patrol nodes.
void DrivingNodes::load_nodemap(std::vector<glm::vec3> positions, std::vector< std::vector<unsigned int> > connections) {
	nodePositions = positions;

	for (unsigned int i = 0; i < connections.size(); i++) {
		nodemap.insert(std::pair<unsigned int, std::vector<unsigned int> >(i, std::vector<unsigned int>()));
		for (unsigned int j : connections[i]) {
			nodemap.at(i).push_back(j);
		}
	}

}

//Computes the shortest path from every node to every other node.
void DrivingNodes::doTheDijkstra() {
	

	//This must be the case to continue
	assert(nodePositions.size() == nodemap.size());

	std::vector<std::vector<float>> knownDistances;

	//Initialize
	for (int i = 0; i < nodemap.size(); i++) {
		knownDistances.push_back(std::vector<float>());
		prev_node.push_back(std::vector<unsigned int>());

		for (int j = 0; j < nodemap.size(); j++) {
			//set the initial distance to 'infinity' unless it is the distance to itself, in which case the distance is 0
			knownDistances[i].push_back(i == j ? 0 : 999999999999.9f);
			prev_node[i].push_back(i == j ? i : NULL);
		}
	}

	//Run algorithm on every node
	for (unsigned int startNode = 0; startNode < nodemap.size(); startNode++) {
		std::vector<unsigned int> visited;
		std::vector<unsigned int> unvisited;
		for (unsigned int i = 0; i < nodemap.size(); i++) unvisited.push_back(i);



		while (unvisited.size() > 0) {
			//get unvisited node with smallest known distance from start node
			int closest_node = unvisited[0];
			for (unsigned int node : unvisited) {
				if (knownDistances[startNode][node] < knownDistances[startNode][closest_node]) closest_node = node;
			}
			//if (closest_node == startNode) printf("%d\n", startNode);

			//for all unvisited nodes from this node, if the total distance from current node is less than the
			//recorded distance, update
			for (unsigned int node : nodemap[closest_node]) {
				if (is_in(node, visited)) continue; // weed out visited nodes

				float distance = x_z_distance(nodePositions[node], nodePositions[closest_node]) + knownDistances[startNode][closest_node];

				if (distance < knownDistances[startNode][node]) {
					knownDistances[startNode][node] = distance;
					prev_node[startNode][node] = closest_node;
				}
				else {
					//printf("%.2f >= %.2f\n", distance, knownDistances[startNode][node]);
				}
			}

			//add current vertex to visited and remove from unvisited
			visited.push_back(closest_node);
			for (int i = 0; i < unvisited.size(); i++) {
				if (unvisited[i] == closest_node) {
					unvisited.erase(unvisited.begin() + i);
					break;
				}
			}
		}
	}
}


//std::stack<glm::vec3> DrivingNodes::getShortestPath(unsigned int a, unsigned int b) {
	//return shortest_paths[a][b];
//}

glm::vec3 DrivingNodes::guideMeFromTo(glm::vec3 a, glm::vec3 b) {

	unsigned int nodeA = getClosestNodeIndex(a);

	if (x_z_distance_squared(a, nodePositions[nodeA]) > 100.0f) return nodePositions[nodeA];

	unsigned int nodeB = getClosestNodeIndex(b);

	return nodePositions[prev_node[nodeB][nodeA]];

}


std::vector<std::vector<glm::vec3> > DrivingNodes::getPatrolRoutes() {
	return patrol_routes;
}

std::map<unsigned int, std::vector<unsigned int> > DrivingNodes::getNodeMap() {
	return nodemap;
}

