#include "Mesh.hpp"

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept {
	aStream <<
		aVertex.position.x << ' ' << aVertex.position.y << ' ' << aVertex.position.z << ' ' <<
		aVertex.normal.x << ' ' << aVertex.position.y << ' ' << aVertex.position.z << ' ' <<
		aVertex.texCoords.x << ' ' << aVertex.texCoords.y;
	return aStream;
}

Mesh::Mesh(VertexArray& aVAO, std::vector<Vertex>& aVerts, std::vector<GLuint>& aInds) noexcept
	: mVBO((GLfloat*)&aVerts[0], aVerts.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT),
	mIBO((GLuint*)&aInds[0], aInds.size())
	{

	std::cout << aVerts.size() << " verts, " << aInds.size() << " inds\n";

	aVAO.bind();
	this->mVBO.bind();
	this->mIBO.bind();

	this->mVBO.enableAttribute(&aVAO, 3, 0, 0);
	this->mVBO.enableAttribute(&aVAO, 3, 1, 3);
	this->mVBO.enableAttribute(&aVAO, 2, 2, 6);
}

Mesh::Mesh(Mesh&& aOther) noexcept
	: mVBO(std::move(aOther.mVBO)), mIBO(std::move(aOther.mIBO)) {}
Mesh& Mesh::operator=(Mesh&& aOther) noexcept {
	this->mVBO = std::move(aOther.mVBO);
	this->mIBO = std::move(aOther.mIBO);

	return *this;
}

void Mesh::draw(Shader& aShader) noexcept {
	aShader.bind();
	this->mVBO.bind();
	this->mIBO.bind();
	this->mIBO.draw();
	//this->mVBO.drawArrays(); //debug
}

Mesh::~Mesh() noexcept {}
