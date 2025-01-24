#ifndef EUROTRAM_LIGHTING
#define EUROTRAM_LIGHTING
#include "Material.hpp"

//we cannot use vec3 since OpenGL's std140 and std430 packings are completely idiotic and pass it as a vec4

//uses: sun
struct alignas(16) Dirlight {
	glm::vec4 direction; //last param ignored
	glm::vec4 color; //last param strength
};

enum LightType : uint8_t {
	LIGHT_TYPE_NORMAL = 0,
	LIGHT_TYPE_FLASHLIGHT,
	LIGHT_TYPE_LEFT_FRONT_VEHICLE,
	LIGHT_TYPE_RIGHT_FRONT_VEHICLE
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

	GLint lightType;
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

	//set position and normalized direction - dirction must be TOWARDS center
	void setPosDirection(const glm::vec3& aPos, const glm::vec3& aDirection = glm::vec3(0.0f, 1.0f, 0.0f)) noexcept;
	//we need a position and center for the shadow map
	void setPosCenter(const glm::vec3& aPos, const glm::vec3& aSceneCenter = glm::vec3(0.0f, 0.0f, 0.0f)) noexcept;
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

//difference from DirectionalShadows -> uses perspecive light matrix
class SpotlightShadows {
public:
	SpotlightShadows(
		const glm::vec3& aPos, const glm::vec3& aDirection,
		const float aNear, const float aFar, const float aFOV
	) noexcept;
	SpotlightShadows(SpotlightShadows& aOther) noexcept = delete;
	SpotlightShadows(SpotlightShadows&& aOther) noexcept;
	SpotlightShadows& operator=(SpotlightShadows& aOther) noexcept = delete;
	SpotlightShadows& operator=(SpotlightShadows&& aOther) noexcept;

	void beginPass(Window& aWindow, UniformMat4& aLightMatrixUniform) noexcept;
	void endPass(Window& aWindow) noexcept;

	//set position and normalized direction - dirction must be TOWARDS center
	void setPosDirection(const glm::vec3& aPos, const glm::vec3& aDirection = glm::vec3(0.0f, 1.0f, 0.0f)) noexcept;
	//we need a position and center for the shadow map
	void setPosCenter(const glm::vec3& aPos, const glm::vec3& aSceneCenter = glm::vec3(0.0f, 0.0f, 0.0f)) noexcept;
	//FOV in degrees!
	void setProjection(const float aNear = 0.1, const float aFar = 30.0, const float aFOV = 45.0) noexcept;

	void bindMap(const uint64_t aTextureSlot) noexcept;

	const glm::mat4& getProjectionMatrix() const noexcept;

	~SpotlightShadows() noexcept;
private:
	Framebuffer mFBO;
	Texture mTexture;

	glm::mat4 mProjection;
	glm::mat4 mView;
	glm::mat4 mLightMatrix;
};

#endif
