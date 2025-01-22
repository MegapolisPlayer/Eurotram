#ifndef EUROTRAM_TEXTURE_IMPL
#define EUROTRAM_TEXTURE_IMPL
#include "Texture.hpp"

template<uint8_t tAmount>
TextureSamplerArray<tAmount>::TextureSamplerArray(const Shader& aShader, const uint64_t aSamplerArrayLocation) noexcept
: mUniform(aShader, aSamplerArrayLocation) {
	for(uint8_t i = 0; i < tAmount; i++) this->mArray[i] = i;
}
template<uint8_t tAmount>
TextureSamplerArray<tAmount>::TextureSamplerArray(const Shader& aShader, const std::string_view aSamplerArrayName) noexcept
: mUniform(aShader, aSamplerArrayName) {
	for(uint8_t i = 0; i < tAmount; i++) this->mArray[i] = i;
}

template<uint8_t tAmount>
void TextureSamplerArray<tAmount>::set() noexcept {
	this->mUniform.set(this->mArray);
}

template<uint8_t tAmount>
UniformIntMultiple<std::array<GLint, tAmount>>& TextureSamplerArray<tAmount>::getUniform() noexcept {
	return this->mUniform;
}

template<uint8_t tAmount>
TextureSamplerArray<tAmount>::~TextureSamplerArray() noexcept {}

#endif
