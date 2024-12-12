#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"

class Model {
public:
	Model(std::string_view aPath) noexcept;

	void draw(Shader& aShader) noexcept;

	~Model() noexcept;
private:
	static Assimp::Importer smModelLoader;

	VertexArray mVAO;
	std::vector<Mesh> mMeshes;
	std::vector <Material> mMaterials;
	std::vector<Texture> mMaterialTextures;

	void processNode(aiNode* apNode, const aiScene* apScene) noexcept;
	void processMesh(Mesh* apMesh, aiMesh* apMeshLoad, const aiScene* apScene) noexcept;
};

#endif
