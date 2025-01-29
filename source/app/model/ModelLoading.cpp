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
glm::vec3 convertToGLM(const aiVector3D& aFrom) noexcept {
	return glm::vec3(aFrom.x, aFrom.y, aFrom.z);
}

Model::Model(std::string_view aPath) noexcept {
	Assimp::Importer mload;
	const aiScene* scene = mload.ReadFile(aPath.data(),
		aiProcess_Triangulate |  aiProcess_SplitLargeMeshes | aiProcess_LimitBoneWeights | aiProcess_OptimizeGraph
	);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << LogLevel::ERROR << "Error: Model load failed! " << mload.GetErrorString() << '\n' << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	glm::mat4 modelTransform = convertToGLM(scene->mRootNode->mTransformation);

	//process meshes and materials
	processNode(scene->mRootNode, scene, modelTransform);

	//process animations
	for(uint64_t i = 0; i < scene->mNumAnimations; i++) {
		processAnimation(scene->mAnimations[i], modelTransform);
	}

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
	this->drawSolidOnly(aUniform);
	this->drawTransparentOnly(aUniform);
}
void Model::drawSolidOnly(UniformMaterial& aUniform) noexcept {
	//range based for loop calls destructor - USE REFERENCES
	for(Mesh& m : this->mMeshes) {
		if(m.mEntry->material.textureOpacity == 1.0) {
			m.draw(aUniform);
		}
	}
}
void Model::drawTransparentOnly(UniformMaterial& aUniform) noexcept {
	for(Mesh& m : this->mMeshes) {
		if(m.mEntry->material.textureOpacity != 1.0) {
			m.draw(aUniform);
		}
	}
}

void Model::setAnimation(std::string_view aAnimationName, const uint64_t aFrame) noexcept {
	for(Animation& a : this->mAnimations) {
		if(a.mName == aAnimationName) {
			a.setStateAtFrame(aFrame);
			break;
		}
	}
}

Model::~Model() noexcept {}

void Model::processNode(aiNode* apNode, const aiScene* apScene, glm::mat4& aTransform) noexcept {
	glm::mat4 nodeOffset = aTransform * convertToGLM(apNode->mTransformation);
	for(uint64_t i = 0; i < apNode->mNumMeshes; i++) {
		this->processMesh(&this->mMeshes, apScene->mMeshes[apNode->mMeshes[i]], apScene, nodeOffset);
	}
	for(uint64_t i = 0; i < apNode->mNumChildren; i++) {
		this->processNode(apNode->mChildren[i], apScene, nodeOffset);
	}
}

void Model::processMesh(std::vector<Mesh>* apMesh, aiMesh* apMeshLoad, const aiScene* apScene, glm::mat4& aTransform) noexcept {
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

		vertices.back().position = glm::vec3(glm::vec4(vertices.back().position, 1.0) * aTransform);

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

	//material
	GMSEntry* texture = nullptr;

	//texture of material
	if(apMeshLoad->mMaterialIndex >= 0) {
		aiMaterial* mat = apScene->mMaterials[apMeshLoad->mMaterialIndex];

		aiString texturePath;
		bool hasTexture = false;

		texture = GlobalMaterialStore::add();

		if(mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
			//has texture and texture path
			hasTexture = true;
			mat->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
			GMSEntry* existingEntry = GlobalMaterialStore::getByPath(texturePath.C_Str());
			if(existingEntry != nullptr) {
				texture = existingEntry;
			}
			else {
				texture->name = mat->GetName().C_Str();
				texture->path = texturePath.C_Str();

				const aiTexture* loaderTexture = apScene->GetEmbeddedTexture(texturePath.C_Str());
				if(loaderTexture != nullptr) {
					//embedded
					texture->texture = Texture(loaderTexture);
				}
				else {
					//in file - load properly
					texture->texture = Texture(texturePath.C_Str());
				}
			}
		}
		else {
			//set path to name - is unique identifier
			texture->path = mat->GetName().C_Str();
		}

		//material values texture
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
		texture->material.brightness = std::max(std::max(tempValue.r, tempValue.g), tempValue.b);
	}
	//no material present - use default purple color
	else {
		texture = GlobalMaterialStore::add();
		texture->name = "UndefinedGMSMaterial";
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

	//if bone new - add to list
	//set weights

	for(uint64_t b = 0; b < apMeshLoad->mNumBones; b++) {
		std::cout << "Bone: " << apMeshLoad->mBones[b]->mName.C_Str() << " of mesh " << apMeshLoad->mName.C_Str() << '\n';

		bool exists = false;
		for(Bone& c : this->mBones) {
			if(std::string(apMeshLoad->mBones[b]->mName.C_Str()) == c.name) {
				exists = true; break;
			}
		}

		if(!exists) {
			//add bone
			this->mBones.push_back({});
			this->mBones.back().name = apMeshLoad->mBones[b]->mName.C_Str();
			this->mBones.back().offset = convertToGLM(apMeshLoad->mBones[b]->mOffsetMatrix);
		}

		//add weights to vertices

		for(uint64_t i = 0; i < apMeshLoad->mBones[b]->mNumWeights; i++) {
			if(apMeshLoad->mBones[b]->mWeights[i].mWeight < 0.05) { continue; }
			//set weight of bones
			for(uint64_t j = 0; j < MAX_BONES_PER_VERTEX; j++) {
				//set first "available" slot
				if(vertices[apMeshLoad->mBones[b]->mWeights[i].mVertexId].boneIds[j] == -1.0) {
					vertices[apMeshLoad->mBones[b]->mWeights[i].mVertexId].boneIds[j] = b;
					vertices[apMeshLoad->mBones[b]->mWeights[i].mVertexId].boneWeights[j] = apMeshLoad->mBones[b]->mWeights[i].mWeight;
				}
			}
		}
	}

	apMesh->emplace_back(vertices, indices, texture); //texture path may be null
}

void Model::processAnimation(aiAnimation* apAnimation, glm::mat4& aTransform) noexcept {
	this->mAnimations.push_back({});
	double duration = apAnimation->mDuration;
	double tps = apAnimation->mTicksPerSecond;

	this->mAnimations.back().mName = apAnimation->mName.C_Str();
	this->mAnimations.back().mTickAmount = duration*tps; //seconds * tps
	this->mAnimations.back().mTicksPerSecond = tps;

	for(uint64_t i = 0; i < apAnimation->mNumChannels; i++) {
		//link to bones
		std::cout << apAnimation->mName.C_Str() << " bone ref: " << apAnimation->mChannels[i]->mNodeName.C_Str() << '\n';

		//keyframes
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumPositionKeys; j++) {
			Keyframe::Position temp;
			temp.position = convertToGLM(apAnimation->mChannels[i]->mPositionKeys[j].mValue);
			temp.frame = (uint64_t)apAnimation->mChannels[i]->mPositionKeys[j].mTime*tps;
			this->mAnimations.back().mPositions.push_back(temp);
		}
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumRotationKeys; j++) {
			Keyframe::Rotation temp;
			temp.rotation = convertToGLM(apAnimation->mChannels[i]->mPositionKeys[j].mValue);
			temp.frame = (uint64_t)apAnimation->mChannels[i]->mPositionKeys[j].mTime*tps;
			this->mAnimations.back().mRotation.push_back(temp);
		}
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumRotationKeys; j++) {
			Keyframe::Scale temp;
			temp.scale = convertToGLM(apAnimation->mChannels[i]->mPositionKeys[j].mValue);
			temp.frame = (uint64_t)apAnimation->mChannels[i]->mPositionKeys[j].mTime*tps;
			this->mAnimations.back().mScale.push_back(temp);
		}
	}
}
