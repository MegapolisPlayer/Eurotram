#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"
#include "Animation.hpp"

constexpr uint64_t MAX_BONES_PER_VERTEX = 4;
constexpr uint64_t STANDARD_MODEL_VERTEX_FLOAT_AMOUNT = 8+MAX_BONES_PER_VERTEX+MAX_BONES_PER_VERTEX;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	GLfloat boneIds[MAX_BONES_PER_VERTEX]; //we save as float but IDs are integers
	GLfloat boneWeights[MAX_BONES_PER_VERTEX];

	Vertex() noexcept {
		for(uint64_t i = 0; i < MAX_BONES_PER_VERTEX; i++) {
			boneIds[i] = -1.0;
			boneWeights[i] = 0.0;
		}
	}
};

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept;

class Mesh {
	friend class Model;
public:
	Mesh() noexcept;

	Mesh(const std::string_view aName, std::vector<Vertex>& aVerts, std::vector<GLuint>& aInds, GMSEntry* aEntry) noexcept;
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
	std::string mName;

	//vao 1 per model
	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;

	GMSEntry* mEntry; //1 entry is enough - variant data in GMS itself, when reset just find one with same name and standard ID
};

#endif
