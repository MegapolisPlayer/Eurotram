#include "ModelLoading.hpp"

glm::mat4 convertToGLM(const fastgltf::math::fmat4x4& aFrom) noexcept {
	glm::mat4 result;

	//convert from column-major to row-major
	//each line - glm row
	result[0][0] = aFrom[0][0]; result[0][1] = aFrom[1][0]; result[0][2] = aFrom[2][0]; result[0][3] = aFrom[3][0];
	result[1][0] = aFrom[0][1]; result[1][1] = aFrom[1][1]; result[1][2] = aFrom[2][1]; result[1][3] = aFrom[3][1];
	result[2][0] = aFrom[0][2]; result[2][1] = aFrom[1][2]; result[2][2] = aFrom[2][2]; result[2][3] = aFrom[3][2];
	result[3][0] = aFrom[0][3]; result[3][1] = aFrom[1][3]; result[3][2] = aFrom[2][3]; result[3][3] = aFrom[3][3];

	return result;
}
glm::quat convertToGLM(const fastgltf::math::quat<float>& aFrom) noexcept {
	return glm::quat(aFrom.x(), aFrom.y(), aFrom.z(), aFrom.w());
}
glm::vec3 convertToGLM(const fastgltf::math::vec<float, 3> aFrom) noexcept {
	return glm::vec3(aFrom.x(), aFrom.y(), aFrom.z());
}
glm::vec4 convertToGLM(const fastgltf::math::vec<float, 4> aFrom) noexcept {
	return glm::vec4(aFrom.x(), aFrom.y(), aFrom.z(), aFrom.w());
}

//temp init of bone matrix buffer
Model::Model(const std::filesystem::path& aPath) noexcept {
	constexpr auto extensions =
		fastgltf::Extensions::KHR_materials_ior |
		fastgltf::Extensions::KHR_materials_specular |
		fastgltf::Extensions::KHR_materials_emissive_strength |
		fastgltf::Extensions::KHR_materials_sheen
	;

	auto gdb = fastgltf::GltfDataBuffer::FromPath(aPath);
	if(!gdb) {
		std::cerr << LogLevel::ERROR << "Error: GLTF2 model load failed!\n" << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}
	fastgltf::Parser parse(extensions);

	constexpr auto importOptions =
		fastgltf::Options::DontRequireValidAssetMember |
		fastgltf::Options::LoadExternalBuffers |
		fastgltf::Options::LoadExternalImages |
		fastgltf::Options::GenerateMeshIndices |
		fastgltf::Options::DecomposeNodeMatrices
	;

	//we need to pass directory
	auto loadState = parse.loadGltfBinary(gdb.get(), aPath.parent_path(), importOptions);
	fastgltf::Asset* model = loadState.get_if();
	if(!model) {
		std::cerr <<
			LogLevel::ERROR << "Error: GLTF2 model data load failed! " <<
			fastgltf::getErrorMessage(loadState.error()) <<
			"(parent path: " <<aPath.parent_path() << ", current: " << aPath << ")\n"
		<< LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	//material
	for(fastgltf::Material& m : model->materials) {
		std::cout << "Material: " << m.name.c_str() << '\n';
		GMSEntry* material = GlobalMaterialStore::add();
		material->name = m.name.c_str();
		material->material.color = convertToGLM(m.pbrData.baseColorFactor);
		material->material.specular = glm::vec4(1.0f); //TODO add specular color read without segfault...
		material->material.ior = m.ior;
		material->material.shininess = m.pbrData.metallicFactor;
		material->material.textureAmount = 0.0f;
		material->material.textureSlot = 0;
		//transparency data stored in colors themselves
		material->material.textureOpacity = 1.0f;
		material->material.brightness = m.emissiveStrength;

		auto& texInfo = m.pbrData.baseColorTexture;
		if(texInfo.has_value()) {
			material->material.textureAmount = 1.0f;
			auto& texture = model->textures[texInfo->textureIndex];
			if(texture.imageIndex.has_value()) {
				auto& image = model->images[texture.imageIndex.value()];

				//for each variant possibility - call correct function

				fastgltf::sources::URI* uriData = std::get_if<fastgltf::sources::URI>(&image.data);
				fastgltf::sources::Array* arrayData = std::get_if<fastgltf::sources::Array>(&image.data);
				fastgltf::sources::BufferView* bufData = std::get_if<fastgltf::sources::BufferView>(&image.data);

				if(arrayData) {
					material->path = m.name.c_str();
					material->texture = Texture(arrayData->bytes.data(), arrayData->bytes.size());
				}
				else if(uriData) {
					material->path = uriData->uri.c_str();
					material->texture = Texture(material->path);
				}
				else if(bufData) {
					//every texture loaded here...
					material->path = m.name.c_str();

					auto& bufferView = model->bufferViews[bufData->bufferViewIndex];
					auto& buffer = model->buffers[bufferView.bufferIndex];

					std::visit(fastgltf::visitor {
						[&](auto& arg) {},
						[&](fastgltf::sources::Array& aData) {
							material->texture = Texture(aData.bytes.data() + bufferView.byteOffset, bufferView.byteLength);
						}
					}, buffer.data);
				}
				else {
					std::cerr << LogLevel::ERROR << "Error: texture type undefined!\n" << LogLevel::RESET;
				}
			}
		}
	}

	std::vector<glm::mat4> meshMatrices;
	meshMatrices.resize(model->meshes.size());
	this->mNodes.reserve(model->nodes.size());
	this->mMeshes.reserve(model->meshes.size());

	//nodes
	for(fastgltf::Node& node : model->nodes) {
		this->mBoneMatrices.push_back(glm::mat4(1.0));

		if(node.skinIndex.has_value()) {
			this->mNodes.emplace_back(node.name.c_str(), convertToGLM(fastgltf::getTransformMatrix(node)), node.skinIndex.value(), this->mBoneMatrices.size()-1);
		}
		else {
			this->mNodes.emplace_back(node.name.c_str(), convertToGLM(fastgltf::getTransformMatrix(node)), -1, -1);
		}

		if(node.meshIndex.has_value()) {
			this->mNodes.back().meshId = node.meshIndex.value();
		}

		std::cout << node.name.c_str() << ' ' << this->mNodes.size()-1 << ')';
		for(auto& c : node.children) {
			std::cout << ' ' << model->nodes[c].name.c_str();
			this->mNodes.back().children.push_back(c);
		}
		std::cout << '\n';
	}

	//we do not know if matrices are sorted properly and we need to apply matrices in the correct order

	//set node parent attribute
	for(uint64_t i = 0; i < this->mNodes.size(); i++) {
		for(int64_t c : this->mNodes[i].children) {
			this->mNodes[c].parent = i; //parent of our children is us
		}
	}

	//calculate transform and inverseBindMatrices
	for(uint64_t n = 0; n < this->mNodes.size(); n++) {
		std::vector<glm::mat4> transform;
		std::vector<glm::mat4> inverseBind;
		uint64_t nodeId = n;

		//get matrices - root first element
		while(true) {
			transform.insert(transform.begin(), this->mNodes[nodeId].localMatrix); //calculate transform from local!
			inverseBind.insert(inverseBind.begin(), this->mNodes[nodeId].inverseBindMatrix);
			if(this->mNodes[nodeId].parent == -1) { break; }
			nodeId = this->mNodes[nodeId].parent;
		}

		//multiply them - from parent to child

		this->mNodes[n].transformMatrix = glm::mat4(1.0f);
		for(glm::mat4& mat : transform) {
			this->mNodes[n].transformMatrix *= mat;
		}
		this->mNodes[n].inverseBindMatrix = glm::mat4(1.0f);
		for(glm::mat4& mat : inverseBind) {
			this->mNodes[n].inverseBindMatrix *= mat;
		}

		if(this->mNodes[n].meshId != -1) {
			meshMatrices[this->mNodes[n].meshId] = this->mNodes[n].transformMatrix;
		}
	}

	//meshes
	uint64_t meshMatrixId = 0;
	for(fastgltf::Mesh& m : model->meshes) {
		//split meshes based on their material
		std::vector<MeshBlueprint> mbp;
		for(fastgltf::Material& mat : model->materials) {
			mbp.push_back({});
			mbp.back().materialName = mat.name.c_str(); //name of mesh is also name of material of mesh
			mbp.back().entry = GlobalMaterialStore::getByName(mat.name.c_str());
		}

		for(fastgltf::Primitive& p : m.primitives) {
			//id of split
			uint64_t meshSplitId = 0;
			for(uint64_t i = 0; i < mbp.size(); i++) {
				if(model->materials[p.materialIndex.value()].name.c_str() == mbp[i].materialName) {
					meshSplitId = i; break;
				}
			}

			//aliases
			auto& vertices = mbp[meshSplitId].vertices;
			auto& indices = mbp[meshSplitId].indices;

			size_t initialId = vertices.size();

			//indices
			{
				fastgltf::Accessor& indicesAccess = model->accessors[p.indicesAccessor.value()];
				indices.reserve(indices.size() + indicesAccess.count);
				fastgltf::iterateAccessor<GLuint>(*model, indicesAccess, [&](GLuint aId) {
					indices.push_back(aId + initialId);
				});
			}

			//vertices
			{
				fastgltf::Accessor& verticesAccess = model->accessors[p.findAttribute("POSITION")->accessorIndex];
				vertices.resize(vertices.size() + verticesAccess.count);
				fastgltf::iterateAccessorWithIndex<glm::vec3>(*model, verticesAccess, [&](glm::vec3 aV, GLuint aId) {
					vertices[aId + initialId].position = glm::vec3(meshMatrices[meshMatrixId] * glm::vec4(aV, 1.0));
				});
			}

			//normals
			{
				fastgltf::Accessor& normalAccess = model->accessors[p.findAttribute("NORMAL")->accessorIndex];
				fastgltf::iterateAccessorWithIndex<glm::vec3>(*model, normalAccess, [&](glm::vec3 aV, GLuint aId) {
					vertices[initialId+aId].normal = aV;
				});
			}

			//UVs
			{
				fastgltf::Accessor& uvAccess = model->accessors[p.findAttribute("TEXCOORD_0")->accessorIndex];
				fastgltf::iterateAccessorWithIndex<glm::vec2>(*model, uvAccess, [&](glm::vec2 aV, GLuint aId) {
					vertices[initialId+aId].texCoords.x = aV.x;
					vertices[initialId+aId].texCoords.y = 1.0 - aV.y; //flipping UVs Y simpler than flipping every image!
				});
			}

			//joints
			{
				fastgltf::Accessor& jointsAccess = model->accessors[p.findAttribute("JOINTS_0")->accessorIndex];
				if(p.findAttribute("JOINTS_0") != p.attributes.end()) {
					fastgltf::iterateAccessorWithIndex<fastgltf::math::u8vec4>(*model, jointsAccess, [&](fastgltf::math::u8vec4 aV, GLuint aId) {
						vertices[initialId+aId].boneIds[0] = (GLfloat)aV.x();
						vertices[initialId+aId].boneIds[1] = (GLfloat)aV.y();
						vertices[initialId+aId].boneIds[2] = (GLfloat)aV.z();
						vertices[initialId+aId].boneIds[3] = (GLfloat)aV.w();
					});
				}
			}

			//weights
			{
				fastgltf::Accessor& jointsAccess = model->accessors[p.findAttribute("WEIGHTS_0")->accessorIndex];
				if(p.findAttribute("WEIGHTS_0") != p.attributes.end()) {
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(*model, jointsAccess, [&](fastgltf::math::fvec4 aV, GLuint aId) {
						vertices[initialId+aId].boneWeights[0] = aV.x();
						vertices[initialId+aId].boneWeights[1] = aV.y();
						vertices[initialId+aId].boneWeights[2] = aV.z();
						vertices[initialId+aId].boneWeights[3] = aV.w();
					});
				}
			}
		}

		meshMatrixId++;

		for(MeshBlueprint& bp : mbp) {
			if(bp.vertices.size() == 0 || bp.indices.size() == 0) continue;
			this->mMeshes.emplace_back(bp);
		}
	}

	//process bones
	for(fastgltf::Skin& s : model->skins) {
		this->mBones.push_back({});
		this->mBones.back().name = s.name.c_str();

		fastgltf::Accessor& ibmAccess =  model->accessors[s.inverseBindMatrices.value()];
		fastgltf::iterateAccessorWithIndex<fastgltf::math::fmat4x4>(*model, ibmAccess, [&](fastgltf::math::fmat4x4 aV, GLuint aId) {
			this->mNodes[aId].inverseBindMatrix = convertToGLM(aV);
			std::cout << this->mBones.back().name << " IBM transform " << aId << ": " << glm::vec3(this->mNodes[aId].inverseBindMatrix * glm::vec4(1.0f)) << '\n';

		});

		for(size_t j : s.joints) {
			this->mBones.back().joints.push_back(j); //nodes!
		}
	}

	//process animations
	for(fastgltf::Animation& a : model->animations) {
		this->mAnimations.emplace_back();
		auto& anim = this->mAnimations.back();
		anim.mName = a.name.c_str();

		for(fastgltf::AnimationSampler& s : a.samplers) {
			anim.mSamplers.push_back({});

			fastgltf::Accessor& samplerInputAccess = model->accessors[s.inputAccessor];
			fastgltf::Accessor& samplerOutputAccess = model->accessors[s.outputAccessor];

			//input - keyframe times

			fastgltf::iterateAccessor<GLfloat>(*model, samplerInputAccess, [&](float aV) {
				anim.mSamplers.back().time.push_back(aV);
			});

			//output - property (vec3 for transform, scale - vec4 for rotation quaternion
			if(samplerOutputAccess.type == fastgltf::AccessorType::Vec3) {
				fastgltf::iterateAccessor<glm::vec3>(*model, samplerOutputAccess, [&](glm::vec3 aV) {
					anim.mSamplers.back().value.push_back(glm::vec4(aV, 1.0));
				});
			}
			else if(samplerOutputAccess.type == fastgltf::AccessorType::Vec4) {
				fastgltf::iterateAccessor<glm::vec4>(*model, samplerOutputAccess, [&](glm::vec4 aV) {
					anim.mSamplers.back().value.push_back(aV);
				});
			}
			else {} //should never happen GLTF stores "keyframes" only as vec3 or vec4
		}
		for(fastgltf::AnimationChannel& c : a.channels) {
			anim.mSamplers[c.samplerIndex].type = c.path;
			if(c.nodeIndex.has_value()) {
				anim.mSamplers[c.samplerIndex].nodeIndex = c.nodeIndex.value();
			}
			else {
				anim.mSamplers[c.samplerIndex].nodeIndex = -1;
			}
		}
	}
}

void Model::addVariant(const std::string_view aMaterialName, const std::string_view aTexturePath, const std::string_view aIdentificator) noexcept {
	GMSEntry* variant = GlobalMaterialStore::copyStandard(aMaterialName);
	variant->texture = Texture(aTexturePath);
	variant->variant = aIdentificator;
}
void Model::addVariant(const std::string_view aMaterialName, const Material& aMaterialRef, const std::string_view aIdentificator) noexcept {
	GMSEntry* variant = GlobalMaterialStore::copyStandard(aMaterialName);
	variant->material = aMaterialRef;
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
	aBoneMatrices.setNewData(this->mBoneMatrices.data(), this->mBoneMatrices.size());
	aBoneMatrices.set();
	for(Mesh& m : this->mMeshes) {
		m.draw(aUniform);
	}
}

void Model::setAnimation(std::string_view aAnimationName, const float aTime) noexcept {
	for(Animation& a : this->mAnimations) {
		if(a.mName == aAnimationName) {
			a.setStateAtTime(*this, aTime);
			return;
		}
	}
	std::cerr << LogLevel::WARNING << "Animation " << aAnimationName << " not found!\n" << LogLevel::RESET;
}

Model::~Model() noexcept {}

//--
//MODEL INSTANCING
//--

ModelInstancer::ModelInstancer() noexcept {

}

void ModelInstancer::drawInstanced(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept {

}

ModelInstancer::~ModelInstancer() noexcept {

}
