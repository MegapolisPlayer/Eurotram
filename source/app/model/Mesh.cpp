#include "Mesh.hpp"

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept {
	aStream <<
		aVertex.position.x << ' ' << aVertex.position.y << ' ' << aVertex.position.z << ' ' <<
		aVertex.normal.x << ' ' << aVertex.position.y << ' ' << aVertex.position.z << ' ' <<
		aVertex.texCoords.x << ' ' << aVertex.texCoords.y;
	return aStream;
}

Mesh::Mesh(std::vector<Vertex>& aVerts, std::vector<GLuint>& aInds, std::string_view aTexturePath) noexcept
	: mVBO((GLfloat*)aVerts.data(), aVerts.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT),
	mIBO((GLuint*)aInds.data(), aInds.size()), mTexture(aTexturePath)
	{

	this->mVAO.bind();
	this->mVBO.bind();
	this->mVBO.enableAttribute(&this->mVAO, 3);
	this->mVBO.enableAttribute(&this->mVAO, 3);
	this->mVBO.enableAttribute(&this->mVAO, 2);
	this->mIBO.bind();
}

Mesh::Mesh(Mesh&& aOther) noexcept
	: mVAO(std::move(aOther.mVAO)), mVBO(std::move(aOther.mVBO)), mIBO(std::move(aOther.mIBO)),
	mTexture(std::move(aOther.mTexture)), mMaterial(std::move(aOther.mMaterial)) {}

Mesh& Mesh::operator=(Mesh&& aOther) noexcept {
	this->mVAO = std::move(aOther.mVAO);
	this->mVBO = std::move(aOther.mVBO);
	this->mIBO = std::move(aOther.mIBO);
	this->mTexture = std::move(aOther.mTexture);
	this->mMaterial = std::move(aOther.mMaterial);

	return *this;
}

void Mesh::draw(UniformMaterial& aUniform) noexcept {
	this->mVAO.bind();
	aUniform.update(&this->mMaterial);
	aUniform.set();
	this->mTexture.bind(0);
	this->mIBO.draw();
	this->mVAO.unbind();
}

Mesh::~Mesh() noexcept {}
