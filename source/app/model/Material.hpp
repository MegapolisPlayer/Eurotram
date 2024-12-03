#ifndef EUROTRAM_MATERIAL
#define EUROTRAM_MATERIAL
#include "../base/Base.hpp"

struct Material {
	float diffuse;
	float specular;
	float roughness;
	float shininess;

	glm::vec3 color;
	float transparency;
};

class UniformMaterial {
public:
	UniformMaterial(
		const Shader* const aShader,
		const uint64_t aValuesLocation,
		const uint64_t aColorLocation
	) noexcept;
	void set(const Material& aValue) noexcept;
private:
	UniformVec4 mUValues;
	UniformVec4 mUColor;
};

#endif
