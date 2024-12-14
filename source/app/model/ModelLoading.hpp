#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"

class Model {
public:
	Model(std::string_view aPath) noexcept;

	void draw(UniformMaterial& aUniform) noexcept;

	~Model() noexcept;
private:
	std::vector<Mesh> mMeshes;

	void processNode(aiNode* apNode, const aiScene* apScene) noexcept;
	void processMesh(std::vector<Mesh>* apMesh, aiMesh* apMeshLoad, const aiScene* apScene) noexcept;
};

#endif
