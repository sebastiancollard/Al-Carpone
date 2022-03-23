#pragma once

#include<vector>
#include<glm/glm.hpp>
#include<map>
#include<stack>

class DrivingNodes {
private:

	//Holds an ordered list of positions. In patrolling, the police car will continually go to the next node in the 
	//vector, looping back to 0 after the end.
	std::vector<std::vector<glm::vec3> > patrol_routes;

	//Holds the connections of every node, ie nodePositions[i] connects to nodePositions[j] for j in nodeMap.at(i)
	std::map<unsigned int, std::vector<unsigned int> > nodemap;
	std::vector<glm::vec3> nodePositions;

	//Contains shortest_paths[i][j] = nextNode from nodePositions[i] to nodePositions[j]
	//Computed by Dijkstra's algorithm
	std::vector< std::vector< unsigned int > > prev_node;

	//Loads all of the patrol routes from a list of every node and a list of all their connectivity. The input positions should
	//only include patrol routes.
	void load_patrol_routes(std::vector<glm::vec3> patrolPositions, std::vector<std::vector<unsigned int> > patrolConnections);

	//Loads a map from every node position and their connections. This should include both patrol nodes and non-patrol nodes.
	void load_nodemap(std::vector<glm::vec3> positions, std::vector< std::vector<unsigned int> > connections);

	//CPSC331 strikes again!
	void doTheDijkstra();

public:

	DrivingNodes(std::vector<glm::vec3> patrolPositions, std::vector<std::vector<unsigned int> > patrolConnections,
		std::vector<glm::vec3> navigationPositions, std::vector<std::vector<unsigned int> > navigationConnections);

	//Returns the index of the node in 'nodePositions' closest to the input position
	unsigned int getClosestNodeIndex(glm::vec3 position);
	glm::vec3 getClosestNodePosition(glm::vec3 position);

	//Returns the next node in the input route that a police car should go to.
	glm::vec3 getNextPatrolNodePosition(unsigned int routeNumber, unsigned int routeSequenceStep);

	void printPatrolRoutes();
	void printNodeMap();
	void printShortestRoutes();

	std::vector<std::vector<glm::vec3> > getPatrolRoutes();
	std::vector<std::vector<std::stack<glm::vec3> > > getShortestPaths();
	glm::vec3 guideMeFromTo(glm::vec3 a, glm::vec3 b);

	void incrementTargetIndex(unsigned int routeId, unsigned int &targetIndex);

	std::map<unsigned int, std::vector<unsigned int> > getNodeMap();

};