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

#endif
