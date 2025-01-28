#include "ModelLoading.hpp"

glm::mat4 convertToGLM(const aiMatrix4x4& aFrom) noexcept {
	glm::mat4 result;
	//abcd in assimp is row - GLM uses more normal structure
	//convert from row-major to column-major
	result[0][0] = aFrom.a1; result[1][0] = aFrom.a2; result[2][0] = aFrom.a3; result[3][0] = aFrom.a4;
	result[0][1] = aFrom.b1; result[1][1] = aFrom.b2; result[2][1] = aFrom.b3; result[3][1] = aFrom.b4;
	result[0][2] = aFrom.c1; result[1][2] = aFrom.c2; result[2][2] = aFrom.c3; result[3][2] = aFrom.c4;
	result[0][3] = aFrom.d1; result[1][3] = aFrom.d2; result[2][3] = aFrom.d3; result[3][3] = aFrom.d4;
	return result;
}

glm::quat convertToGLM(const aiQuaternion& aFrom) noexcept {
	return glm::quat(aFrom.w, aFrom.x, aFrom.y, aFrom.z);
}

Model::Model(std::string_view aPath) noexcept {
	Assimp::Importer mload;
	const aiScene* scene = mload.ReadFile(aPath.data(), aiProcess_Triangulate);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << LogLevel::ERROR << "Error: Model load failed! " << mload.GetErrorString() << '\n' << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	//process meshes and materials
	processNode(scene->mRootNode, scene);

	//process animations
	for(uint64_t i = 0; i < scene->mNumAnimations; i++) {
		processAnimation(scene->mAnimations[i]);
	}

	this->mBoneOffsets = ShaderBuffer(,);

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

	//material

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
			}
			else {
				texture->texture = Texture(texturePath.C_Str());
				texture->path = texturePath.C_Str();

				//sample only if new texture

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
		}
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

	//bones and weights

	std::vector<Bone> bones;
	for(uint64_t b = 0; b < apMeshLoad->mNumBones; b++) {
		bones.push_back({});
		bones.back().name = apMeshLoad->mBones[b]->mName.C_Str();
		bones.back().offset = convertToGLM(apMeshLoad->mBones[b]->mOffsetMatrix);

		for(uint64_t i = 0; i < apMeshLoad->mBones[b]->mNumWeights; i++) {
			if(apMeshLoad->mBones[b]->mWeights[i].mWeight < 0.01) { continue; }
			for(uint64_t j = 0; j < MAX_BONES_PER_VERTEX; j++) {
				if(vertices[apMeshLoad->mBones[b]->mWeights[i].mVertexId].boneIds[j] != -1.0) {
					vertices[apMeshLoad->mBones[b]->mWeights[i].mVertexId].boneIds[j] = b;
					vertices[apMeshLoad->mBones[b]->mWeights[i].mVertexId].boneWeights[j] = apMeshLoad->mBones[b]->mWeights[i].mWeight;
				}
			}
		}
	}

	apMesh->emplace_back(vertices, indices, texture, bones); //texture path may be null
}

void Model::processAnimation(aiAnimation* apAnimation) noexcept {
	this->mAnimations.push_back({});
	double duration = apAnimation->mDuration;
	double tps = apAnimation->mTicksPerSecond;

	this->mAnimations.back().mTickAmount = (uint64_t)duration/tps;

	for(uint64_t i = 0; i < apAnimation->mNumChannels; i++) {
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumPositionKeys; j++) {

		}
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumRotationKeys; j++) {

		}
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumRotationKeys; j++) {

		}
	}
}
