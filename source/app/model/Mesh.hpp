#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept;

#define STANDARD_MODEL_VERTEX_FLOAT_AMOUNT 8

class Mesh {
	friend class Model;
public:
	Mesh(std::vector<Vertex>& aVerts, std::vector<GLuint>& aInds, std::string_view aTexturePath) noexcept;
	Mesh(Mesh&& aOther) noexcept;
	Mesh& operator=(Mesh&& aOther) noexcept;
	Mesh(Mesh& aOther) noexcept = delete;
	Mesh& operator=(Mesh& aOther) noexcept = delete;

	void draw(UniformMaterial& aUniform) noexcept;

	~Mesh() noexcept;
private:
	//vao 1 per model
	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;

	Material mMaterial;
	Texture mTexture; //we only expect one - combined diffuse + opacity
};

#endif
