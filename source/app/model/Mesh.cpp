#include "Mesh.hpp"

Mesh::Mesh() noexcept
	: mVAO(false), mVBO(nullptr, 0, 0), mIBO(nullptr, 0), mEntry(nullptr) {}

Mesh::Mesh(const std::string_view aName, const std::vector<Vertex>& aVerts, const std::vector<GLuint>& aInds, GMSEntry* aEntry) noexcept
	: mName(aName.data()), mVBO((GLfloat*)aVerts.data(), aVerts.size(), STANDARD_MODEL_VERTEX_FLOAT_AMOUNT),
	mIBO((GLuint*)aInds.data(), aInds.size()), mEntry(aEntry) {
	this->mVAO.bind();
	this->mVBO.bind();

	this->mVBO.enableStandardAttributes(&this->mVAO);

	this->mIBO.bind();
}

Mesh::Mesh(const MeshBlueprint& aBP) noexcept : Mesh(aBP.name, aBP.vertices, aBP.indices, aBP.entry) {}

Mesh::Mesh(Mesh&& aOther) noexcept
	: mName(std::move(aOther.mName)), mVAO(std::move(aOther.mVAO)), mVBO(std::move(aOther.mVBO)), mIBO(std::move(aOther.mIBO)),
	mEntry(aOther.mEntry) {}

Mesh& Mesh::operator=(Mesh&& aOther) noexcept {
	this->mName = std::move(aOther.mName);
	this->mVAO = std::move(aOther.mVAO);
	this->mVBO = std::move(aOther.mVBO);
	this->mIBO = std::move(aOther.mIBO);
	this->mEntry = std::move(aOther.mEntry);
	aOther.mEntry = nullptr;

	return *this;
}

void Mesh::draw(UniformMaterial& aUniform) noexcept {
	this->mVAO.bind();
	aUniform.update(&this->mEntry->material);
	aUniform.set();
	this->mEntry->texture.bind(0);
	//this->mVBO.drawPoints();
	this->mIBO.draw();
	this->mVAO.unbind();
}

void Mesh::setEntry(GMSEntry* aEntry) noexcept {
	if(aEntry->name != this->mEntry->name || aEntry == nullptr) {
		std::cerr << LogLevel::ERROR << "Attempt to assign different material - create a variant instead.\n" << LogLevel::RESET;
		return;
	}
	this->mEntry = aEntry;
}
void Mesh::resetEntry() noexcept {
	this->mEntry = GlobalMaterialStore::get(this->mEntry->name, GMS_STANDARD_IDENTIFICATOR);
}

Mesh::~Mesh() noexcept {}
