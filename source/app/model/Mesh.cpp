#include "Mesh.hpp"

Mesh::Mesh() noexcept
	: mVBO(NULL, 0, 0), mIBO(nullptr, 0) {}
void Mesh::draw(Shader& aShader) noexcept {
	aShader.bind();
	this->mIBO.draw();
}

void Mesh::make() noexcept {
	//manual destructor calls
	this->mVBO.~VertexBuffer();
	this->mIBO.~IndexBuffer();

	//recreate

	new(&this->mVBO)VertexBuffer(
		(GLfloat*)&this->mVertices[0],
		this->mVertices.size(),
		STANDARD_MODEL_VERTEX_FLOAT_AMOUNT
	);

	new(&this->mIBO)IndexBuffer(
		(GLuint*)&this->mIndices[0],
		this->mIndices.size()
	);
}

Mesh::~Mesh() noexcept {}
