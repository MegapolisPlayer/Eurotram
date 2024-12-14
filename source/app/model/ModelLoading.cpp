#include "ModelLoading.hpp"

Assimp::Importer Model::smModelLoader = {};

Model::Model(std::string_view aPath, VertexArray& aVAO) noexcept {
	const aiScene* scene = smModelLoader.ReadFile(
		aPath.data(), aiProcess_Triangulate | aiProcess_FlipUVs
	);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << LogLevel::ERROR << "Error: Model load failed! " << smModelLoader.GetErrorString() << '\n' << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	std::cout << "Model file read!\n";

	processNode(scene->mRootNode, scene, aVAO);
}

void Model::draw(Shader& aShader) noexcept {
	//range based for loop calls destructor - USE REFERENCE!!!
	for(Mesh& m : this->mMeshes) {
		m.draw(aShader);
	}
}
Model::~Model() noexcept {

}

void Model::processNode(aiNode* apNode, const aiScene* apScene, VertexArray& aVAO) noexcept {
	//TODO later make use of parent-child system to apply translations

	for(uint64_t i = 0; i < apNode->mNumMeshes; i++) {
		this->processMesh(&this->mMeshes, apScene->mMeshes[apNode->mMeshes[i]], apScene, aVAO);
	}
	for(uint64_t i = 0; i < apNode->mNumChildren; i++) {
		this->processNode(apNode->mChildren[i], apScene, aVAO);
	}
}
void Model::processMesh(std::vector<Mesh>* apMesh, aiMesh* apMeshLoad, const aiScene* apScene, VertexArray& aVAO) noexcept {
	std::cout << "Processing mesh with v = " << apMeshLoad->mNumVertices << "\n";
	//vertices
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	vertices.reserve(apMeshLoad->mNumVertices);
	indices.reserve(apMeshLoad->mNumFaces*3);
	for(uint64_t v = 0; v < apMeshLoad->mNumVertices; v++) {
		vertices.push_back({});

		glm::vec3 vec;
		vec.x = apMeshLoad->mVertices[v].x;
		vec.y = apMeshLoad->mVertices[v].y;
		vec.z = apMeshLoad->mVertices[v].z;
		vertices.back().position = vec;

		vec.x = apMeshLoad->mNormals[v].x;
		vec.y = apMeshLoad->mNormals[v].y;
		vec.z = apMeshLoad->mNormals[v].z;
		vertices.back().normal = vec;

		//if textures present set texture coords, otherwise set to -1
		if(apMeshLoad->mTextureCoords[0]) {
			vertices.back().texCoords = {
				apMeshLoad->mTextureCoords[0][v].x,
				apMeshLoad->mTextureCoords[0][v].y
			};
		}
		else {
			vertices.back().texCoords = glm::vec2(-1.0f);
		}
	}

	//indices (we load as faces)
	for(uint64_t f = 0; f < apMeshLoad->mNumFaces; f++) {
		aiFace fc = apMeshLoad->mFaces[f];
		for(uint64_t i = 0; i < fc.mNumIndices; i++) {
			indices.push_back(fc.mIndices[i]);
		}
	}

	//materials - if contains load
	if(apMeshLoad->mMaterialIndex >= 0) {
		aiMaterial* mat = apScene->mMaterials[apMeshLoad->mMaterialIndex];
		if(mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
			//opacity and ambient texture are in same file
			aiString texturePath;
			mat->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);

			bool skipTexture = false;
			for(const Texture& t : this->mMaterialTextures) {
				//if texture already loaded - skip
				if(t.getPath() == texturePath.C_Str()) {
					skipTexture = true;
					break;
				}
			}
			if(!skipTexture) {
				//constructor loads texture
				this->mMaterialTextures.emplace_back(texturePath.C_Str());
			}
		}
	}

	//setup mesh from data
	apMesh->emplace_back(aVAO, vertices, indices);
}
