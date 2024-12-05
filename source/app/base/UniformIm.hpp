#ifndef EUROTRAM_UNIFORM_IMPL
#define EUROTRAM_UNIFORM_IMPL
#include "Uniform.hpp"

template<typename tType>
Uniform<tType>::Uniform(const Shader* const aShader, const char* aUniformName) noexcept {
		this->mHandle = glGetUniformLocation(aShader->getHandle(), aUniformName);
		if(this->mHandle == -1) {
			std::cerr << LogLevel::ERROR << "Could not find uniform \"" << aUniformName << "\"\n" << LogLevel::RESET;
			std::exit(EXIT_FAILURE);
		}
}
template<typename tType>
Uniform<tType>::Uniform(const Shader* const aShader, const uint64_t aLocationOverride) noexcept {
	this->mHandle = aLocationOverride;
}


template<typename tType>
GLint Uniform<tType>::getHandle() const noexcept { return this->mHandle; }

template<typename tType>
StructUniform<tType>::StructUniform(const uint64_t aLocation) noexcept
	: mBuffer(NULL, sizeof(tType)), mLocation(aLocation) {
	this->mBuffer.bind(this->mLocation);
}
template<typename tType>
void StructUniform<tType>::update(const tType* const aValue) noexcept {
	this->mBuffer.update(aValue, sizeof(tType));
}
template<typename tType>
void StructUniform<tType>::set() noexcept {
	this->mBuffer.bind(this->mLocation);
}
template<typename tType>
StructUniform<tType>::~StructUniform() noexcept {}

#endif
