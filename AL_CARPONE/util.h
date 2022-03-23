#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

static bool is_in(unsigned int key, std::vector<unsigned int>& vector) {
	for (unsigned int elem : vector) if (elem == key) return true;
	return false;
}

//distance between vec3s ignoring height (y)
static float x_z_distance(glm::vec3 a, glm::vec3 b) {
	float x = b.x - a.x;
	float z = b.z - a.z;
	return sqrt(x * x + z * z);
}


static std::vector<glm::vec3> load_positions(std::string path) {
	std::vector<std::string> lines;
	std::string line;

	std::vector<glm::vec3> positions;

	std::ifstream input(path);

	if (!input.is_open()) {
		printf("Error opening file: %s\n", path.c_str());
		return positions;
	}

	while (getline(input, line)) if (line[0] == 'v' && line[1] == ' ') lines.push_back(line.substr(2));

	for (std::string l : lines) {
		std::vector<float> positions_raw;
		std::string numberStr;

		for (char c : l) {
			if (c == ' ' && numberStr.size() > 0) {
				float num;
				std::stringstream(numberStr) >> num;

				positions_raw.push_back(num);
				numberStr.clear();
			}
			else numberStr.push_back(c);
		}
		float num;
		std::stringstream(numberStr) >> num;

		positions_raw.push_back(num);
		numberStr.clear();

		positions.push_back(glm::vec3(positions_raw[0], positions_raw[1], positions_raw[2]));
		positions_raw.clear();
	}

	return positions;
}

static std::vector<std::vector<unsigned int>> load_edges(std::string path) {
	std::vector<std::string> lines;
	std::string line;

	std::vector<std::vector<unsigned int>> edges;

	std::ifstream input(path);

	if (!input.is_open()) {
		printf("Error opening file: %s\n", path.c_str());
		return edges;
	}

	//get each line starting with "l " 
	while (getline(input, line)) if (line[0] == 'l' && line[1] == ' ') lines.push_back(line.substr(2));


	for (std::string l : lines) {

		std::string index_str = "";
		unsigned int start_index = -1;

		//std::cout << "WORKING LINE: " + l << std::endl;

		for (char c : l) {

			if (c != ' ') index_str += c;

			//Get the start index
			else if (index_str != "" && start_index == -1) {
				start_index = atoi(index_str.c_str()) - 1;
				//std::cout << "START_INDEX: " + start_index << std::endl;
				index_str = "";
				//make sure theres a spot to put the edges

				//printf("NEW ENTRY: edges.size() = %d\n", (int)edges.size());
			}
		}
		//Now holds the second of the pair
		if (index_str != "") {
			unsigned int index = atoi(index_str.c_str()) - 1;
			//std::cout << "INDEX: " + index << std::endl;
			index_str = "";

			while (edges.size() < (fmax(start_index, index) + 1)) edges.push_back(std::vector<unsigned int>{});

			//Making sure we dont have this edge and its not a self edge

			//printf("ACCESSING edges[%d], where edges.size() = %d\n", start_index, (int)edges.size());
			if (index != start_index && !is_in(index, edges[start_index])) {
				edges[start_index].push_back(index);
			}
			if (start_index != index && !is_in(start_index, edges[index])) {
				edges[index].push_back(start_index);
			}

		}
	}

	return edges;
}

