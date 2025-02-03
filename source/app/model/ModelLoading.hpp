#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"
#include "Animation.hpp"

glm::mat4 convertToGLM(const fastgltf::math::fmat4x4& aFrom) noexcept;
glm::quat convertToGLM(const fastgltf::math::quat<float>& aFrom) noexcept;
glm::vec3 convertToGLM(const fastgltf::math::vec<float, 3> aFrom) noexcept;
glm::vec4 convertToGLM(const fastgltf::math::vec<float, 4> aFrom) noexcept;

struct GLTFNode {
	std::string name;
	glm::mat4 transformMatrix = glm::mat4(1.0f);
	glm::mat4 localMatrix = glm::mat4(1.0f);
	glm::mat4 inverseBindMatrix = glm::mat4(1.0f);
	int64_t idOfSkin = -1;
	int64_t boneOutputMatrixId = -1;
	int64_t meshId = -1;

	std::vector<uint64_t> children;
	int64_t parent = -1; //-1 if root

	GLTFNode() noexcept {};
	GLTFNode(const std::string& aName, const glm::mat4& aLocal, const int64_t aIdOfSkin, const int64_t aBoneOutputMatrixId) noexcept
		: name(aName), localMatrix(aLocal), idOfSkin(aIdOfSkin), boneOutputMatrixId(aBoneOutputMatrixId) {};
	~GLTFNode() noexcept {};
};

class Model {
	friend class Animation;
public:
	Model(const std::filesystem::path& aPath) noexcept;

	//creates copy of existing material (its standard variant) and adds different texture
	//Standard identificator defined as MODEL_STANDARD_IDENTIFICATOR
	void addVariant(const std::string_view aMaterialName, const std::string_view aTexturePath, const std::string_view aIdentificator) noexcept;
	//non-textured variant
	void addVariant(const std::string_view aMaterialName, const Material& aMaterialRef, const std::string_view aIdentificator) noexcept;

	void setVariant(const std::string_view aMaterialName, const std::string_view aIdentificator) noexcept;
	void resetVariant(const std::string_view aMaterialName) noexcept;

	void draw(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept;

	void setAnimation(std::string_view aAnimationName, const float aTime) noexcept;

	~Model() noexcept;
private:
	std::vector<Mesh> mMeshes;
	std::vector<Animation> mAnimations;

	std::vector<Skin> mBones;
	std::vector<GLTFNode> mNodes; //pair of node world transform and id of skin
	std::vector<glm::mat4> mBoneMatrices;
};

class ModelInstancer {
public:
	ModelInstancer() noexcept;

	void drawInstanced(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept;

	~ModelInstancer() noexcept;
private:
	std::vector<glm::mat4> mTransforms;
};

#endif
