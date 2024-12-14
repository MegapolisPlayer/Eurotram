#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"

class Model {
public:
	Model(std::string_view aPath, VertexArray& aVAO) noexcept;

	void draw(Shader& aShader) noexcept;

	~Model() noexcept;
private:
	static Assimp::Importer smModelLoader;

	VertexArray mVAO;
	std::vector<Mesh> mMeshes;
	std::vector <Material> mMaterials;
	std::vector<Texture> mMaterialTextures;

	void processNode(aiNode* apNode, const aiScene* apScene, VertexArray& aVAO) noexcept;
	void processMesh(std::vector<Mesh>* apMesh, aiMesh* apMeshLoad, const aiScene* apScene, VertexArray& aVAO) noexcept;
};

#endif
