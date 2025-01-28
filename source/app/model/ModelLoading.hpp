#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"
#include "Animation.hpp"

glm::mat4 convertToGLM(const aiMatrix4x4& aFrom) noexcept;
glm::quat convertToGLM(const aiQuaternion& aFrom) noexcept;

class Model {
public:
	Model(std::string_view aPath) noexcept;

	//creates copy of existing material (its standard variant) and adds different texture
	//Standard identificator defined as MODEL_STANDARD_IDENTIFICATOR
	void addVariant(const std::string_view aMaterialName, const std::string_view aTexturePath, const std::string_view aIdentificator) noexcept;

	void setVariant(const std::string_view aMaterialName, const std::string_view aIdentificator) noexcept;
	void resetVariant(const std::string_view aMaterialName) noexcept;

	void draw(UniformMaterial& aUniform) noexcept;
	void drawSolidOnly(UniformMaterial& aUniform) noexcept;

	//void setAnimation() noexcept;

	~Model() noexcept;
private:
	std::vector<Mesh> mMeshes;
	std::vector<Animation> mAnimations;

	void processNode(aiNode* apNode, const aiScene* apScene) noexcept;
	void processMesh(std::vector<Mesh>* apMesh, aiMesh* apMeshLoad, const aiScene* apScene) noexcept;

	void processAnimation(aiAnimation* apAnimation) noexcept;
};

#endif
