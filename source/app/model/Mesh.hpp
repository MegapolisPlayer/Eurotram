#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"
#include "Animation.hpp"

struct MeshBlueprint {
	std::string name;
	std::string materialName; //every mesh has to have 1 material
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	GMSEntry* entry;
};

class Mesh {
	friend class Model;
public:
	Mesh() noexcept;
	Mesh(const std::string_view aName, const std::vector<Vertex>& aVerts, const std::vector<GLuint>& aInds, GMSEntry* aEntry) noexcept;
	Mesh(const MeshBlueprint& aBP) noexcept;

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
