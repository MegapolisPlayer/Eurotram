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

//temp init of bone matrix buffer
Model::Model(std::string_view aPath) noexcept {
	Assimp::Importer mload;
	const aiScene* scene = mload.ReadFile(aPath.data(),
		aiProcess_Triangulate |  aiProcess_SplitLargeMeshes | aiProcess_LimitBoneWeights |
		aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_ValidateDataStructure | aiProcess_PopulateArmatureData
	);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << LogLevel::ERROR << "Error: Model load failed! " << mload.GetErrorString() << '\n' << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	//process meshes, materials, transforms and bones with weights

	std::vector<BoneChildrenData> bpd;
	bpd.reserve(100);
	this->mBones.reserve(50); //TODO rewrite pointer system since this is invalidated if more bones
	this->mBoneMatrices.reserve(50);

	this->mMeshes.resize(scene->mNumMeshes);

	glm::mat4 modelTransform = convertToGLM(scene->mRootNode->mTransformation);
	processNode(nullptr, scene->mRootNode, scene, modelTransform, bpd);

	//set parents of bones
	//for each parent of each bone - find and set pointer

	std::cout << "Bone amount: " << this->mBones.size() << " BPD " << bpd.size() << '\n';

	for(uint64_t i = 0; i < this->mBones.size(); i++) {
		//for each child name
		for(uint64_t j = 0; j < bpd[i].size(); j++) {
			std::cout << bpd[i][j] << '/';
			//find id
			for(uint64_t f = 0; f < this->mBones.size(); f++) {
				if(this->mBones[f].name == bpd[i][j]) {
					this->mBones[f].parent = &this->mBones[i]; //set parent of child to our node
					std::cout << '\n' <<this->mBones[f].name << ">>>" << this->mBones[i].name << '\n';
					std::cout << "---(" <<this->mBones[f].parent->name << ">>>" << ((this->mBones[i].parent == nullptr) ? "null" : this->mBones[i].parent->name) << ")\n";
				}
			}
			std::cout << '\n';
		}
	}

	//process animations
	for(uint64_t i = 0; i < scene->mNumAnimations; i++) {
		processAnimation(scene->mAnimations[i], scene, modelTransform);
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

void Model::draw(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept {
	this->drawSolidOnly(aUniform, aBoneMatrices);
	this->drawTransparentOnly(aUniform, aBoneMatrices);
}
void Model::drawSolidOnly(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept {
	//range based for loop calls destructor - USE REFERENCES
	aBoneMatrices.setNewData(this->mBoneMatrices.data(), this->mBoneMatrices.size());
	aBoneMatrices.set();
	for(Mesh& m : this->mMeshes) {
		if(m.mEntry->material.textureOpacity == 1.0) {
			m.draw(aUniform);
		}
	}
}
void Model::drawTransparentOnly(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept {
	aBoneMatrices.setNewData(this->mBoneMatrices.data(), this->mBoneMatrices.size());
	aBoneMatrices.set();
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
			return;
		}
	}
	std::cerr << LogLevel::WARNING << "Animation " << aAnimationName << " not found!\n" << LogLevel::RESET;
}

Model::~Model() noexcept {}

void Model::processNode(aiNode* apParent, aiNode* apNode, const aiScene* apScene, glm::mat4& aTransform, std::vector<BoneChildrenData>& aBoneParentVector) noexcept {
	glm::mat4 nodeOffset = aTransform * convertToGLM(apNode->mTransformation);
	for(uint64_t i = 0; i < apNode->mNumMeshes; i++) {
		this->processMesh(apNode, apNode->mMeshes[i], apScene, nodeOffset, aBoneParentVector);
	}
	for(uint64_t i = 0; i < apNode->mNumChildren; i++) {
		this->processNode(apNode, apNode->mChildren[i], apScene, nodeOffset, aBoneParentVector);
	}

}

void Model::processMesh(aiNode* apParent, const uint64_t aMeshId, const aiScene* apScene, glm::mat4& aTransform, std::vector<BoneChildrenData>& aBoneParentVector) noexcept {
	//vertices
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	aiMesh* mesh = apScene->mMeshes[aMeshId];

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces*3);

	for(uint64_t v = 0; v < mesh->mNumVertices; v++) {
		vertices.push_back({});

		vertices.back().position.x = mesh->mVertices[v].x;
		vertices.back().position.y = mesh->mVertices[v].y;
		vertices.back().position.z = mesh->mVertices[v].z;

		vertices.back().position = glm::vec3(glm::vec4(vertices.back().position, 1.0) * aTransform);

		vertices.back().normal.x = mesh->mNormals[v].x;
		vertices.back().normal.y = mesh->mNormals[v].y;
		vertices.back().normal.z = mesh->mNormals[v].z;

		//if textures present set texture coords, otherwise set to -1
		if(mesh->mTextureCoords[0]) {
			vertices.back().texCoords.x = mesh->mTextureCoords[0][v].x,
			vertices.back().texCoords.y = mesh->mTextureCoords[0][v].y;
		}
		else { vertices.back().texCoords = glm::vec2(-1.0f); }
	}

	//indices (we load as faces)
	for(uint64_t f = 0; f < mesh->mNumFaces; f++) {
		aiFace fc = mesh->mFaces[f];
		if(fc.mNumIndices < 3) { continue; } //ignore small faces (2 and 1 )

		for(uint64_t i = 0; i < fc.mNumIndices; i++) {
			indices.push_back(fc.mIndices[i]);
		}
	}

	//material
	GMSEntry* texture = nullptr;

	//texture of material
	if(mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = apScene->mMaterials[mesh->mMaterialIndex];

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

	//add weights of bones

	for(uint64_t b = 0; b < mesh->mNumBones; b++) {
		bool exists = false;

		//check if bone exists
		for(Bone& c : this->mBones) {
			if(mesh->mBones[b]->mName.C_Str() == c.name) {
				exists = true; break;
			}
		}

		if(!exists) {
			//add bone
			this->mBones.push_back({});
			this->mBoneMatrices.emplace_back(glm::mat4(1.0f));
			aBoneParentVector.push_back({});

			this->mBones.back().name = mesh->mBones[b]->mName.C_Str();
			this->mBones.back().offset = convertToGLM(mesh->mBones[b]->mOffsetMatrix);
			this->mBones.back().local = convertToGLM(mesh->mBones[b]->mNode->mTransformation);
			this->mBones.back().transformation = &this->mBoneMatrices.back();

			for(uint64_t k = 0; k < mesh->mBones[b]->mNode->mNumChildren; k++) {
				aBoneParentVector.back().push_back(mesh->mBones[b]->mNode->mChildren[k]->mName.C_Str());
			}
		}

		//add weights to vertices

		for(uint64_t i = 0; i < mesh->mBones[b]->mNumWeights; i++) {
			//if basically no weight
			if(mesh->mBones[b]->mWeights[i].mWeight < 0.05) { continue; }
			//set weight of bones
			for(uint64_t j = 0; j < MAX_BONES_PER_VERTEX; j++) {
				//set first "available" slot
				if(vertices[mesh->mBones[b]->mWeights[i].mVertexId].boneIds[j] <= -0.01) {
					vertices[mesh->mBones[b]->mWeights[i].mVertexId].boneIds[j] = b;
					vertices[mesh->mBones[b]->mWeights[i].mVertexId].boneWeights[j] = mesh->mBones[b]->mWeights[i].mWeight;
					break;
				}
			}
		}
	}

	this->mMeshes[aMeshId] = std::move(Mesh(mesh->mName.C_Str(), vertices, indices, texture)); //texture path may be null
}



void Model::processAnimation(aiAnimation* apAnimation, const aiScene* apScene, glm::mat4& aTransform) noexcept {
	this->mAnimations.push_back({});

	this->mAnimations.back().mName = apAnimation->mName.C_Str();
	this->mAnimations.back().mTickAmount = apAnimation->mDuration/ANIMATION_FRAME_PER_FRAME_AMOUNT; //in ticks!
	this->mAnimations.back().mTicksPerSecond = apAnimation->mTicksPerSecond;

	std::cout << "DUR " << this->mAnimations.back().mTickAmount << " TPS" << apAnimation->mTicksPerSecond << '\n';

	for(uint64_t i = 0; i < apAnimation->mNumChannels; i++) {
		//link to bones
		for(Bone& b : this->mBones) {
			if(b.name == apAnimation->mChannels[i]->mNodeName.C_Str()) {
				//found our bone - add it
				this->mAnimations.back().addBone(&b);
			}
		}

		//keyframes
		std::cout << "KF amount " << apAnimation->mChannels[i]->mNumPositionKeys << "\n";
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumPositionKeys; j++) {
			Keyframe::Position temp;
			temp.position = convertToGLM(apAnimation->mChannels[i]->mPositionKeys[j].mValue);
			std::cout << "KF" << apAnimation->mChannels[i]->mNodeName.C_Str() << "@" << (uint64_t)(apAnimation->mChannels[i]->mPositionKeys[j].mTime/ANIMATION_FRAME_PER_FRAME_AMOUNT) << ":" << temp.position << '\n';
			temp.frame = apAnimation->mChannels[i]->mPositionKeys[j].mTime/ANIMATION_FRAME_PER_FRAME_AMOUNT;
			this->mAnimations.back().mPositions.push_back(temp);
		}
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumRotationKeys; j++) {
			Keyframe::Rotation temp;
			temp.rotation = convertToGLM(apAnimation->mChannels[i]->mRotationKeys[j].mValue);
			temp.frame = apAnimation->mChannels[i]->mRotationKeys[j].mTime/ANIMATION_FRAME_PER_FRAME_AMOUNT;
			this->mAnimations.back().mRotation.push_back(temp);
		}
		for(uint64_t j = 0; j < apAnimation->mChannels[i]->mNumScalingKeys; j++) {
			Keyframe::Scale temp;
			temp.scale = convertToGLM(apAnimation->mChannels[i]->mScalingKeys[j].mValue);
			temp.frame = apAnimation->mChannels[i]->mScalingKeys[j].mTime/ANIMATION_FRAME_PER_FRAME_AMOUNT;
			this->mAnimations.back().mScale.push_back(temp);
		}
	}
}
