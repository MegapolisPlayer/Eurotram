#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	GLfloat materialId; //id of material
	GLfloat textureId; //id of texture - can be different
};

#define STANDARD_MODEL_VERTEX_FLOAT_AMOUNT 10

class Mesh {
	friend class Model;
public:
	Mesh() noexcept;

	void draw(Shader& aShader) noexcept;

	//(re-)creates VBO, IBO and transform
	void make() noexcept;

	~Mesh() noexcept;
private:
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;

	//vao 1 per model
	VertexBuffer mVBO;
	IndexBuffer mIBO;
};

#endif
