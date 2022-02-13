#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>



std::vector<glm::vec3> load_positions(std::string path) {
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