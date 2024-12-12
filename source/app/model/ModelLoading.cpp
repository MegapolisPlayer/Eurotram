#include "ModelLoading.hpp"

Assimp::Importer Model::smModelLoader = {};

Model::Model(std::string_view aPath) noexcept {
	const aiScene* scene = smModelLoader.ReadFile(
		aPath.data(), aiProcess_Triangulate | aiProcess_FlipUVs
	);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << LogLevel::ERROR << "Error: Model load failed!" << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}


}

void Model::draw(Shader& aShader) noexcept {
	for(Mesh m : this->mMeshes) {
		m.draw(aShader);
	}
}
Model::~Model() noexcept {

}

void Model::processNode(aiNode* apNode, const aiScene* apScene) noexcept {
	//TODO later make use of parent-child system to apply translations

	for(uint64_t i = 0; i < apNode->mNumMeshes; i++) {
		this->mMeshes.push_back({});
		this->processMesh(&this->mMeshes.back(), apScene->mMeshes[apNode->mMeshes[i]], apScene);
		aiMesh* mesh = apScene->mMeshes[apNode->mMeshes[i]];
	}
	for(uint64_t i = 0; i < apNode->mNumChildren; i++) {
		this->processNode(apNode->mChildren[i], apScene);
	}
}
void Model::processMesh(Mesh* apMesh, aiMesh* apMeshLoad, const aiScene* apScene) noexcept {
	//vertices
	apMesh->mVertices.reserve(apMeshLoad->mNumVertices);
	for(uint64_t v = 0; v < apMeshLoad->mNumVertices; v++) {
		apMesh->mVertices.push_back({});

		//pos and normals
		apMesh->mVertices.back().position = {
			apMeshLoad->mVertices[v].x,
			apMeshLoad->mVertices[v].y,
			apMeshLoad->mVertices[v].z
		};
		apMesh->mVertices.back().normal = {
			apMeshLoad->mNormals[v].x,
			apMeshLoad->mNormals[v].y,
			apMeshLoad->mNormals[v].z
		};

		//if textures present set texture coords, otherwise set to -1
		if(apMeshLoad->mTextureCoords[0]) {
			apMesh->mVertices.back().texCoords = {
				apMeshLoad->mTextureCoords[0][v].x,
				apMeshLoad->mTextureCoords[0][v].y
			};
		}
		else {
			apMesh->mVertices.back().texCoords = glm::vec2(0.0f);
		}
	}

	//indices (we load as faces)
	for(uint64_t f = 0; f < apMeshLoad->mNumFaces; f++) {
		for(uint64_t i = 0; i < apMeshLoad->mFaces[f].mNumIndices; i++) {
			apMesh->mIndices.push_back(apMeshLoad->mFaces[f].mIndices[i]);
		}
	}

	//materials - if contains load
	if(apMeshLoad->mMaterialIndex >= 0) {
		//TODO finish
	}

	//setup mesh form data
	apMesh->make();
}
