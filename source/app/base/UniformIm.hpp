#ifndef EUROTRAM_UNIFORM_IMPL
#define EUROTRAM_UNIFORM_IMPL
#include "Uniform.hpp"

template<typename tType>
Uniform<tType>::Uniform(const Shader& aShader, const std::string_view aUniformName) noexcept {
		this->mHandle = glGetUniformLocation(aShader.getHandle(), aUniformName.data());
		if(this->mHandle == -1) {
			std::cerr << LogLevel::ERROR << "Could not find uniform \"" << aUniformName << "\"\n" << LogLevel::RESET;
			std::exit(EXIT_FAILURE);
		}
}
template<typename tType>
Uniform<tType>::Uniform(const Shader& aShader, const uint64_t aLocationOverride) noexcept {
	this->mHandle = aLocationOverride;
}

template<typename tType>
GLint Uniform<tType>::getHandle() const noexcept { return this->mHandle; }

template<typename tType>
StructUniform<tType>::StructUniform(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: mBuffer(NULL, sizeof(tType)*aAmount), mLocation(aLocation), mAmount(aAmount) {
	this->mBuffer.bind(this->mLocation);
}
template<typename tType>
void StructUniform<tType>::setNewData(const tType* const aValue, const uint64_t aAmount) {
	this->mBuffer.bind(this->mLocation);
	this->mBuffer.setNewData(aValue, sizeof(tType)*aAmount);
}
template<typename tType>
void StructUniform<tType>::update(const tType* const aValue, const uint64_t aFirstElem, const uint64_t aLastElem) noexcept {
	this->mBuffer.bind(this->mLocation);
	if(aLastElem == 0) {
		//whole buffer
		this->mBuffer.update(aValue, sizeof(tType)*(this->mAmount-aFirstElem), sizeof(tType)*aFirstElem);
	}
	else {
		this->mBuffer.update(aValue, sizeof(tType)*(aLastElem-aFirstElem), sizeof(tType)*aFirstElem);
	}
}
template<typename tType>
void StructUniform<tType>::set() noexcept {
	this->mBuffer.bind(this->mLocation);
}
template<typename tType>
StructUniform<tType>::~StructUniform() noexcept {}

template <ContiguousStandardNotationContainer tType>
UniformIntMultiple<tType>::UniformIntMultiple(const Shader& aShader, const uint64_t aLocationOverride) noexcept
: Uniform<tType>(aShader, aLocationOverride) {}

template <ContiguousStandardNotationContainer tType>
UniformIntMultiple<tType>::UniformIntMultiple(const Shader& aShader, const std::string_view aUniformName) noexcept
: Uniform<tType>(aShader, aUniformName) {}

template <ContiguousStandardNotationContainer tType>
void UniformIntMultiple<tType>::set(tType aValue) noexcept {
	glUniform1iv(this->mHandle, aValue.size(), aValue.data());
}

#endif
