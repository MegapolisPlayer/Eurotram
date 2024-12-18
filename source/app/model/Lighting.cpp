#include "Lighting.hpp"

void setSpotlightRadius(Spotlight* aSpotlight, const float aRadius, const float aCutoffRange) noexcept {
	aSpotlight->radius = glm::cos(glm::radians(aRadius));
	aSpotlight->inner = glm::cos(glm::radians(aRadius-(aCutoffRange/2.0)));
	aSpotlight->outer = glm::cos(glm::radians(aRadius+(aCutoffRange/2.0)));
}

void setAttenuation(const AttenuationValues aDistance, float* aConstant, float* aLinear, float* aQuadratic) noexcept {
	*aConstant = 1.0;
	switch(aDistance) {
		case AttenuationValues::DISTANCE_7:
			*aLinear = 0.7;
			*aQuadratic = 1.8;
			return;
		case AttenuationValues::DISTANCE_20:
			*aLinear = 0.22;
			*aQuadratic = 0.20;
			return;
		case AttenuationValues::DISTANCE_50:
			*aLinear = 0.09;
			*aQuadratic = 0.032;
			return;
		case AttenuationValues::DISTANCE_100:
			*aLinear = 0.045;
			*aQuadratic = 0.0075;
			return;
		case AttenuationValues::DISTANCE_200:
			*aLinear = 0.022;
			*aQuadratic = 0.0019;
			return;
	}
}

UniformDirlight::UniformDirlight(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: StructUniform(aLocation, aAmount) {}
UniformDirlight::~UniformDirlight() noexcept {}

UniformPointlight::UniformPointlight(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: StructUniform(aLocation, aAmount) {}
UniformPointlight::~UniformPointlight() noexcept {}

UniformSpotlight::UniformSpotlight(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: StructUniform(aLocation, aAmount) {}
UniformSpotlight::~UniformSpotlight() noexcept {}
