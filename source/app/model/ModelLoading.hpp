#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"
#include "Animation.hpp"

glm::mat4 convertToGLM(const fastgltf::math::fmat4x4& aFrom) noexcept;
glm::quat convertToGLM(const fastgltf::math::quat<float>& aFrom) noexcept;
glm::vec3 convertToGLM(const fastgltf::math::vec<float, 3> aFrom) noexcept;
glm::vec4 convertToGLM(const fastgltf::math::vec<float, 4> aFrom) noexcept;

class Model {
public:
	Model(const std::filesystem::path& aPath) noexcept;

	//creates copy of existing material (its standard variant) and adds different texture
	//Standard identificator defined as MODEL_STANDARD_IDENTIFICATOR
	void addVariant(const std::string_view aMaterialName, const std::string_view aTexturePath, const std::string_view aIdentificator) noexcept;

	void setVariant(const std::string_view aMaterialName, const std::string_view aIdentificator) noexcept;
	void resetVariant(const std::string_view aMaterialName) noexcept;

	void draw(UniformMaterial& aUniform, StructUniform<glm::mat4>& aBoneMatrices) noexcept;

	void setAnimation(std::string_view aAnimationName, const uint64_t aFrame) noexcept;

	~Model() noexcept;
private:
	std::vector<std::pair<glm::mat4, int64_t>> mNodeTransforms;
	std::vector<Mesh> mMeshes;
	std::vector<Skin> mBones;
	std::vector<Animation> mAnimations;
	std::vector<glm::mat4> mBoneMatrices;
};

class ModelInstancer {
public:
	ModelInstancer() noexcept;
	~ModelInstancer() noexcept;
private:
	std::vector<glm::mat4> mTransforms;
};

#endif
