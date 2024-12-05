#ifndef EUROTRAM_LIGHTING
#define EUROTRAM_LIGHTING
#include "Material.hpp"

//uses: sun
struct Dirlight {
	glm::vec3 direction;
	glm::vec3 color;
	float ambient;
};

//uses: lights inside vehicle(s)
struct Pointlight {
	glm::vec3 position;
	glm::vec3 color;
	float ambient;

	float constant;
	float linear;
	float quadratic;
};

//uses: street lamps, headlights...
struct Spotlight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float ambient;

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
