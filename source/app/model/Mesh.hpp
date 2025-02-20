#ifndef EUROTRAM_MESH
#define EUROTRAM_MESH
#include "Sky.hpp"
#include "Animation.hpp"

struct MeshBlueprint {
	std::string name;
	std::string materialName; //every mesh has to have 1 material
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

class Mesh {
	friend class Model;
public:
	Mesh() noexcept;
	Mesh(const std::string_view aName, const std::vector<Vertex>& aVerts, const std::vector<GLuint>& aInds) noexcept;
	Mesh(const MeshBlueprint& aBP) noexcept;

	Mesh(Mesh&& aOther) noexcept;
	Mesh& operator=(Mesh&& aOther) noexcept;
	Mesh(Mesh& aOther) noexcept = delete;
	Mesh& operator=(Mesh& aOther) noexcept = delete;

	void draw(UniformMat4& aTransformUniform, UniformMat3& aNormalUniform) noexcept;
	void drawInstanced(UniformMat4& aTransformUniform, UniformMat3& aNormalUniform, const uint64_t aCount) noexcept;

	//DO NOT set entry with different material name
	void setEntry(GMSEntry* aEntry) noexcept;
	void resetEntry() noexcept;

	std::string getName() const noexcept;
	Transform& getTransform() noexcept;

	~Mesh() noexcept;
private:
	std::string mName;

	//vao 1 per model
	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;

	//material id stored in vertex info

	Transform mModel; //model matrix
};

#endif
