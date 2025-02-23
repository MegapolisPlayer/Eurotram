#include "Mesh.hpp"

Mesh::Mesh() noexcept
	: mVAO(false), mVBO(nullptr, 0, 0), mIBO(nullptr, 0) {}

Mesh::Mesh(const std::string_view aName, const std::vector<Vertex>& aVerts, const std::vector<GLuint>& aInds) noexcept
	: mName(aName.data()), mVBO((GLfloat*)aVerts.data(), aVerts.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT),
	mIBO((GLuint*)aInds.data(), aInds.size()) {
	this->mVAO.bind();
	this->mVBO.bind();

	this->mVBO.enableStandardAttributes(&this->mVAO);

	this->mIBO.bind();
}

Mesh::Mesh(const MeshBlueprint& aBP) noexcept : Mesh(aBP.name, aBP.vertices, aBP.indices) {}

Mesh::Mesh(Mesh&& aOther) noexcept
	: mName(std::move(aOther.mName)), mVAO(std::move(aOther.mVAO)), mVBO(std::move(aOther.mVBO)), mIBO(std::move(aOther.mIBO)) {}

Mesh& Mesh::operator=(Mesh&& aOther) noexcept {
	this->mName = std::move(aOther.mName);
	this->mVAO = std::move(aOther.mVAO);
	this->mVBO = std::move(aOther.mVBO);
	this->mIBO = std::move(aOther.mIBO);

	return *this;
}

//material values bound in model

void Mesh::draw(UniformMat4* aTransformUniform, UniformMat3* aNormalUniform) noexcept {
	this->mVAO.bind();
	if(aTransformUniform) {
		aTransformUniform->set(this->mModel.getMatrix());
	}
	if(aNormalUniform) {
		aNormalUniform->set(this->mModel.getNormalMatrix());
	}
	//this->mVBO.drawPoints();
	this->mIBO.draw();
	this->mVAO.unbind();
}
void Mesh::drawInstanced(const uint64_t aCount, UniformMat4* aTransformUniform, UniformMat3* aNormalUniform) noexcept {
	this->mVAO.bind();
	if(aTransformUniform) {
		aTransformUniform->set(this->mModel.getMatrix());
	}
	if(aNormalUniform) {
		aNormalUniform->set(this->mModel.getNormalMatrix());
	}
	//this->mVBO.drawPoints();
	this->mIBO.drawInstanced(aCount);
	this->mVAO.unbind();
}

std::string Mesh::getName() const noexcept {
	return this->mName;
}
Transform& Mesh::getTransform() noexcept {
	return this->mModel;
}

Mesh::~Mesh() noexcept {}
