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
		if(this->mMeshes[i].mEntry->material.textureOpacity != 1.0) {
			std::iter_swap(this->mMeshes.begin()+i, (this->mMeshes.end()-1-swapCounter));
		}
	}
}

void Model::addVariant(const std::string_view aMaterialName, const std::string_view aTexturePath, const std::string_view aIdentificator) noexcept {
	GMSEntry* variant = GlobalMaterialStore::copyStandard(aMaterialName);
	variant->texture = Texture(aTexturePath);
	variant->variant = aIdentificator;
}
void Model::setVariant(const std::string_view aMaterialName, const std::string_view aIdentificator) noexcept {
	for(Mesh& m : mMeshes) {
		//only the meshes with the material name
		if(m.mEntry->name == aMaterialName) { m.setEntry(GlobalMaterialStore::get(aMaterialName, aIdentificator)); }
	}
}
void Model::resetVariant(const std::string_view aMaterialName) noexcept {
	for(Mesh& m : mMeshes) {
		//only the meshes with the material name
		if(m.mEntry->name == aMaterialName) { m.resetEntry(); }
	}
}

void Model::draw(UniformMaterial& aUniform) noexcept {
	//range based for loop calls destructor - USE REFERENCE!!!
	for(Mesh& m : this->mMeshes) {
		m.draw(aUniform);
	}
}
void Model::drawSolidOnly(UniformMaterial& aUniform) noexcept {
	for(Mesh& m : this->mMeshes) {
		if(m.mEntry->material.textureOpacity == 1.0) {
			m.draw(aUniform);
		}
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

	GMSEntry* texture = GlobalMaterialStore::add();
	apMesh->emplace_back(vertices, indices, texture); //texture path may be null

	//texture of material
	if(apMeshLoad->mMaterialIndex >= 0) {
		aiMaterial* mat = apScene->mMaterials[apMeshLoad->mMaterialIndex];
		texture->name = mat->GetName().C_Str();

		aiString texturePath;
		bool hasTexture = false;

		if(mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			hasTexture = true;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			GMSEntry* existingEntry = GlobalMaterialStore::getByPath(texturePath.C_Str());
			if(existingEntry) {
				texture = existingEntry;
				apMesh->back().mEntry = existingEntry; //same texture = same entry = same material in blender (usually, at least)
			}
			else {
				texture->texture = Texture(texturePath.C_Str());
				texture->path = texturePath.C_Str();
				std::cout << texture->name << '\n';
			}
		}

		aiColor3D tempValue;
		float opacity;

		mat->Get(AI_MATKEY_OPACITY, opacity);
		texture->material.textureOpacity = opacity;

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, tempValue);
		texture->material.color.x = tempValue.r;
		texture->material.color.y = tempValue.g;
		texture->material.color.z = tempValue.b;
		texture->material.color.w = 1.0;

		mat->Get(AI_MATKEY_COLOR_SPECULAR, tempValue);
		texture->material.specular.x = tempValue.r;
		texture->material.specular.y = tempValue.g;
		texture->material.specular.z = tempValue.b;
		texture->material.specular.w = 1.0;

		mat->Get(AI_MATKEY_REFRACTI,  texture->material.ior);

		mat->Get(AI_MATKEY_SHININESS, texture->material.shininess);

		texture->material.textureSlot = 0;
		texture->material.textureAmount = (float)hasTexture;
		mat->Get(AI_MATKEY_COLOR_EMISSIVE, tempValue);
		texture->material.brightness =
			std::max(std::max(tempValue.r, tempValue.g), tempValue.b);
	}
	//no material present - use default purple color
	else {
		//material properties
		texture->material.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		texture->material.specular = glm::vec4(0.5f);
		texture->material.shininess = 0.0f;
		texture->material.ior = 1.0f;
		texture->material.textureAmount = 0.0f; //1.0 texture only, 0.0 color only
		texture->material.textureSlot = 0;
		texture->material.textureOpacity = 1.0f;
		texture->material.brightness = 0.0f; //below this brightness render as normal, above it is brighter
	}
}

