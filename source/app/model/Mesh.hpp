#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"

constexpr uint64_t MAX_BONES_PER_VERTEX = 4;
constexpr uint64_t STANDARD_MODEL_VERTEX_FLOAT_AMOUNT = 8+MAX_BONES_PER_VERTEX+MAX_BONES_PER_VERTEX;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	GLfloat boneIds[MAX_BONES_PER_VERTEX]; //we save as float but IDs are integers
	GLfloat boneWeights[MAX_BONES_PER_VERTEX];
};

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept;

class Mesh {
	friend class Model;
public:
	Mesh(std::vector<Vertex>& aVerts, std::vector<GLuint>& aInds, GMSEntry* aEntry) noexcept;
	Mesh(Mesh&& aOther) noexcept;
	Mesh& operator=(Mesh&& aOther) noexcept;
	Mesh(Mesh& aOther) noexcept = delete;
	Mesh& operator=(Mesh& aOther) noexcept = delete;

	void draw(UniformMaterial& aUniform) noexcept;

	 //DO NOT set entry with different material name
	void setEntry(GMSEntry* aEntry) noexcept;
	void resetEntry() noexcept;

	~Mesh() noexcept;
private:
	//vao 1 per model
	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;

	GMSEntry* mEntry; //1 entry is enough - variant data in GMS itself, when reset just find one with same name and standard ID
};

#endif
