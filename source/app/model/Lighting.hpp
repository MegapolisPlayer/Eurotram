#ifndef EUROTRAM_LIGHTING
#define EUROTRAM_LIGHTING
#include "Material.hpp"

//we cannot use vec3 since OpenGL's std140 and std430 packings are completely idiotic and pass it as a vec4

//uses: sun
struct Dirlight {
	glm::vec4 direction; //last param ignored
	glm::vec4 color; //last param strength
};

//uses: lights inside vehicle(s)
struct Pointlight {
	glm::vec4 position; //last param ignored
	glm::vec4 color; //last param strength

	float constant;
	float linear;
	float quadratic;
};

//uses: street lamps, headlights...
struct Spotlight {
	glm::vec4 position; //last param ignored
	glm::vec4 direction; //last param ignored
	glm::vec4 color; //last param strength

	float constant;
	float linear;
	float quadratic;
};

class UniformDirlight : public StructUniform<Dirlight> {
public:
	UniformDirlight(const uint64_t aLocation) noexcept;
	virtual ~UniformDirlight() noexcept;
private:
};

class UniformPointlight  : public StructUniform<Pointlight> {
public:
	UniformPointlight(const uint64_t aLocation) noexcept;
	virtual ~UniformPointlight() noexcept;
private:
};

class UniformSpotlight : public StructUniform<Spotlight> {
public:
	UniformSpotlight(const uint64_t aLocation) noexcept;
	virtual ~UniformSpotlight() noexcept;
private:
};

#endif
