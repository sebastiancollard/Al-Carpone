#include"Vertex.h"

Vertex::Vertex() {}

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoord) :
	pos(pos), normal(normal), texCoord(texCoord) {}