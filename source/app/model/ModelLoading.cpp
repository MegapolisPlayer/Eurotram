#include "ModelLoading.hpp"

Model::Model(std::string_view aPath) noexcept {
	Assimp::Importer mload;
	const aiScene* scene = mload.ReadFile(aPath.data(), aiProcess_Triangulate);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << LogLevel::ERROR << "Error: Model load failed! " << mload.GetErrorString() << '\n' << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	processNode(scene->mRootNode, scene);

	//move transparent objects to back
	uint64_t swapCounter = 0;
	for(uint64_t i = 0; i < this->mMeshes.size(); i++) {
		if(this->mMeshes[i].mMaterial.textureOpacity != 1.0) {
			std::iter_swap(this->mMeshes.begin()+i, (this->mMeshes.end()-1-swapCounter));
		}
	}
}

void Model::draw(UniformMaterial& aUniform) noexcept {
	//range based for loop calls destructor - USE REFERENCE!!!
	for(Mesh& m : this->mMeshes) {
		m.draw(aUniform);
	}
}
Model::~Model() noexcept {}

void Model::processNode(aiNode* apNode, const aiScene* apScene) noexcept {
	//TODO later make use of parent-child system to apply translations

	for(uint64_t i = 0; i < apNode->mNumMeshes; i++) {
		this->processMesh(&this->mMeshes, apScene->mMeshes[apNode->mMeshes[i]], apScene);
	}
	for(uint64_t i = 0; i < apNode->mNumChildren; i++) {
		this->processNode(apNode->mChildren[i], apScene);
	}
}
void Model::processMesh(std::vector<Mesh>* apMesh, aiMesh* apMeshLoad, const aiScene* apScene) noexcept {
	//vertices
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	vertices.reserve(apMeshLoad->mNumVertices);
	indices.reserve(apMeshLoad->mNumFaces*3);

	for(uint64_t v = 0; v < apMeshLoad->mNumVertices; v++) {
		vertices.push_back({});

		vertices.back().position.x = apMeshLoad->mVertices[v].x;
		vertices.back().position.y = apMeshLoad->mVertices[v].y;
		vertices.back().position.z = apMeshLoad->mVertices[v].z;

		vertices.back().normal.x = apMeshLoad->mNormals[v].x;
		vertices.back().normal.y = apMeshLoad->mNormals[v].y;
		vertices.back().normal.z = apMeshLoad->mNormals[v].z;

		//if textures present set texture coords, otherwise set to -1
		if(apMeshLoad->mTextureCoords[0]) {
			vertices.back().texCoords.x = apMeshLoad->mTextureCoords[0][v].x,
			vertices.back().texCoords.y = apMeshLoad->mTextureCoords[0][v].y;
		}
		else { vertices.back().texCoords = glm::vec2(-1.0f); }
	}

	//indices (we load as faces)
	for(uint64_t f = 0; f < apMeshLoad->mNumFaces; f++) {
		aiFace fc = apMeshLoad->mFaces[f];
		if(fc.mNumIndices < 3) { continue; } //ignore small faces (2 and 1 )

		for(uint64_t i = 0; i < fc.mNumIndices; i++) {
			indices.push_back(fc.mIndices[i]);
		}
	}

	//texture of material
	if(apMeshLoad->mMaterialIndex >= 0) {
		aiMaterial* mat = apScene->mMaterials[apMeshLoad->mMaterialIndex];

		aiString texturePath;
		bool hasTexture = false;

		if(mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			hasTexture = true;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
		}

		apMesh->emplace_back(vertices, indices, texturePath.C_Str()); //texture path may be null

		aiColor3D tempValue;
		float opacity;

		mat->Get(AI_MATKEY_OPACITY, opacity);
		apMesh->back().mMaterial.textureOpacity = opacity;

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, tempValue);
		apMesh->back().mMaterial.color.x = tempValue.r;
		apMesh->back().mMaterial.color.y = tempValue.g;
		apMesh->back().mMaterial.color.z = tempValue.b;
		apMesh->back().mMaterial.color.w = 1.0;

		mat->Get(AI_MATKEY_COLOR_SPECULAR, tempValue);
		apMesh->back().mMaterial.specular.x = tempValue.r;
		apMesh->back().mMaterial.specular.y = tempValue.g;
		apMesh->back().mMaterial.specular.z = tempValue.b;
		apMesh->back().mMaterial.specular.w = 1.0;

		mat->Get(AI_MATKEY_REFRACTI,  apMesh->back().mMaterial.ior);

		mat->Get(AI_MATKEY_SHININESS, apMesh->back().mMaterial.shininess);

		apMesh->back().mMaterial.textureSlot = 0;
		apMesh->back().mMaterial.textureAmount = (float)hasTexture;
		mat->Get(AI_MATKEY_COLOR_EMISSIVE, tempValue);
		apMesh->back().mMaterial.brightness =
			std::max(std::max(tempValue.r, tempValue.g), tempValue.b);
	}
	//no material present - use default purple color
	else {
		//setup mesh from data
		apMesh->emplace_back(vertices, indices, "");

		//material properties
		apMesh->back().mMaterial.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		apMesh->back().mMaterial.specular = glm::vec4(0.5f);
		apMesh->back().mMaterial.shininess = 0.0f;
		apMesh->back().mMaterial.ior = 1.0f;
		apMesh->back().mMaterial.textureAmount = 0.0f; //1.0 texture only, 0.0 color only
		apMesh->back().mMaterial.textureSlot = 0;
		apMesh->back().mMaterial.textureOpacity = 1.0f;
		apMesh->back().mMaterial.brightness = 0.0f; //below this brightness render as normal, above it is brighter
	}
}

