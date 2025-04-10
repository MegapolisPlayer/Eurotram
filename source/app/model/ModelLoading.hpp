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
	int64_t idOfSkin = -1;
	int64_t meshId = -1;

	std::vector<uint64_t> children;
	int64_t parent = -1; //-1 if root

	GLTFNode() noexcept {};
	GLTFNode(const std::string& aName, const glm::mat4 aGlobal, const glm::mat4& aLocal, const int64_t aIdOfSkin) noexcept
		: name(aName), transformMatrix(aGlobal), localMatrix(aLocal), idOfSkin(aIdOfSkin) {};
	~GLTFNode() noexcept {};
};

struct Skin {
	std::string name;
	std::vector<uint64_t> joints;
	std::vector<glm::mat4> inverseBindMatrix;
	int64_t skeleton = -1;
};

class Model {
	friend class Animation;
public:
	Model(const std::filesystem::path& aPath) noexcept;

	Model(Model&) noexcept = delete;
	Model(Model&& aOther) noexcept;
	Model& operator=(Model&) noexcept = delete;
	Model& operator=(Model&& aOther) noexcept;

	//creates copy of existing material (its standard variant) and adds different texture
	//Standard identificator defined as MODEL_STANDARD_IDENTIFICATOR
	void addVariant(const std::string_view aMaterialName, const std::string_view aTexturePath, const std::string_view aIdentifier) noexcept;
	//non-textured variant
	void addVariant(const std::string_view aMaterialName, const Material& aMaterialRef, const std::string_view aIdentifier) noexcept;

	void setVariant(const std::string_view aMaterialName, const std::string_view aIdentifier) noexcept;
	void resetVariant(const std::string_view aMaterialName) noexcept;

	void sendAnimationDataToShader(StructUniform<glm::mat4>& aBoneMatrices, const bool aRecalcAnim = true) noexcept;

	void draw(
		UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices,
		UniformMat4* aTransformUniform = nullptr, UniformMat3* aNormalUniform = nullptr
	) noexcept;
	void drawInstanced(
		UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices, const uint64_t aCount,
		UniformMat4* aTransformUniform = nullptr, UniformMat3* aNormalUniform = nullptr
	) noexcept;

	void setAnimation(std::string_view aAnimationName, const float aTime) noexcept;

	//GLTF limitation - single mesh may only have a single armature
	//workaround: list bones to update and update just them
	//TODO

	void updateAnimation(StructUniform<glm::mat4>& aBoneMatrices) noexcept; //called automatically on draw

	Transform& getGlobalTransform() noexcept;
	void refreshTransforms() noexcept; //updates transforms for all meshes

	Mesh* getMesh(const std::string_view aName) noexcept;

	~Model() noexcept;
private:
	std::vector<Mesh> mMeshes;
	std::vector<Animation> mAnimations;

	std::vector<Skin> mBones;
	std::vector<GLTFNode> mNodes; //pair of node world transform and id of skin
	std::vector<glm::mat4> mOutput;

	uint64_t mFirstGMSMaterial;
	uint64_t mLastGMSMaterial;

	glm::mat4 getNewNodeTransform(GLTFNode& aNode) noexcept;
	void updateJoint(GLTFNode& aNode, StructUniform<glm::mat4>& aBoneMatrices) noexcept;

	float mHeight;
};

#endif
