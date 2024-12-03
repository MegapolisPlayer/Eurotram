#include "Material.hpp"

UniformMaterial::UniformMaterial(
	const Shader* const aShader,
	const uint64_t aValuesLocation,
	const uint64_t aColorLocation
) noexcept : mUValues(aShader, aValuesLocation), mUColor(aShader, aColorLocation) {}

void UniformMaterial::set(const Material& aValue) noexcept {
	this->mUValues.set({aValue.diffuse, aValue.specular, aValue.roughness, aValue.shininess});
	this->mUColor.set({aValue.color, aValue.transparency});
}
