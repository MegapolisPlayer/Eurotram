#ifndef EUROTRAM_LIGHTING
#define EUROTRAM_LIGHTING
#include "Material.hpp"

//we cannot use vec3 since OpenGL's std140 and std430 packings are completely idiotic and pass it as a vec4

//uses: sun
struct alignas(16) Dirlight {
	glm::vec4 direction; //last param ignored
	glm::vec4 color; //last param strength
};

//uses: lights inside vehicle(s)
struct alignas(16) Pointlight {
	glm::vec4 position; //last param ignored
	glm::vec4 color; //last param strength

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
};

//uses: street lamps, headlights...
struct alignas(16) Spotlight {
	glm::vec4 position; //last param ignored
	glm::vec4 direction; //last param ignored
	glm::vec4 color; //last param strength
	GLfloat radius; //as cosine value of angle in radians
	GLfloat inner; //as cosine value of angle in radians
	GLfloat outer; //as cosine value of angle in radians

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
};

void setSpotlightRadius(Spotlight* aSpotlight, const float aRadius, const float aCutoffRange) noexcept;

//set attenuation values (and therefore, distance) for light
enum class AttenuationValues : uint8_t {
	DISTANCE_7,
	DISTANCE_20,
	DISTANCE_50,
	DISTANCE_100,
	DISTANCE_200
};
void setAttenuation(const AttenuationValues aDistance, float* aConstant, float* aLinear, float* aQuadratic) noexcept;

class UniformDirlight : public StructUniform<Dirlight> {
public:
	UniformDirlight(const uint64_t aLocation, const uint64_t aAmount = 1) noexcept;
	virtual ~UniformDirlight() noexcept;
private:
};

class UniformPointlight  : public StructUniform<Pointlight> {
public:
	UniformPointlight(const uint64_t aLocation, const uint64_t aAmount = 1) noexcept;
	virtual ~UniformPointlight() noexcept;
private:
};

class UniformSpotlight : public StructUniform<Spotlight> {
public:
	UniformSpotlight(const uint64_t aLocation, const uint64_t aAmount = 1) noexcept;
	virtual ~UniformSpotlight() noexcept;
private:
};

// shadows

constexpr uint64_t DirectionalShadowMapWidth = 2048;
constexpr uint64_t DirectionalShadowMapHeight = 2048;

class DirectionalShadows {
public:
	DirectionalShadows(
		const glm::vec3& aPos, const glm::vec3& aSceneCenter,
		const float aProjectionSize, const float aNear, const float aFar
	) noexcept;
	DirectionalShadows(DirectionalShadows& aOther) noexcept = delete;
	DirectionalShadows(DirectionalShadows&& aOther) noexcept;
	DirectionalShadows& operator=(DirectionalShadows& aOther) noexcept = delete;
	DirectionalShadows& operator=(DirectionalShadows&& aOther) noexcept;

	void beginPass(Window& aWindow, UniformMat4& aLightMatrixUniform) noexcept;
	void endPass(Window& aWindow) noexcept;

	//we need a position and center for the shadow map
	void setPos(const glm::vec3& aPos, const glm::vec3& aSceneCenter = glm::vec3(0.0f, 0.0f, 0.0f)) noexcept;
	void setProjection(const float aProjectionSize, const float aNear = 0.1, const float aFar = 30.0) noexcept;

	void bindMap(const uint64_t aTextureSlot) noexcept;

	const glm::mat4& getProjectionMatrix() const noexcept;

	~DirectionalShadows() noexcept;
private:
	Framebuffer mFBO;
	Texture mTexture;

	glm::mat4 mProjection;
	glm::mat4 mView;
	glm::mat4 mLightMatrix;
};

constexpr uint64_t StandardShadowMapWidth = 2048;
constexpr uint64_t StandardShadowMapHeight = 2048;

class SpotlightShadows {
public:

private:

};

#endif
