#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	uint64_t materialId;
	uint64_t textureId;
};

class Mesh {
public:
private:
};

#endif
