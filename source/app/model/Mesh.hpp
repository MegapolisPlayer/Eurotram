#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	uint64_t materialId; //id of material
	uint64_t textureId; //id of texture - can be different
};

class Mesh {
public:
	Mesh() noexcept;

	//TODO get vector handles, push back to them
	//in constructor - only reserve amount

	void draw() noexcept;

	~Mesh() noexcept;
private:
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector <Material> mMaterials;
	std::vector<Texture> mMaterialTextures;

};

#endif
