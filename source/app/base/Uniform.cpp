#include "Uniform.hpp"
#include "UniformIm.hpp"

UniformMat4::UniformMat4(const Shader* const aShader, const uint64_t aLocationOverride) noexcept
	: Uniform(aShader, aLocationOverride) {}
UniformMat4::UniformMat4(const Shader* const aShader, const char* aUniformName) noexcept
	: Uniform(aShader, aUniformName) {}
void UniformMat4::set(const glm::mat4& aValue) noexcept {
	glUniformMatrix4fv(this->mHandle, 1, GL_FALSE, glm::value_ptr(aValue));
}

UniformMat3::UniformMat3(const Shader* const aShader, const uint64_t aLocationOverride) noexcept
: Uniform(aShader, aLocationOverride) {}
UniformMat3::UniformMat3(const Shader* const aShader, const char* aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformMat3::set(const glm::mat3& aValue) noexcept {
	glUniformMatrix3fv(this->mHandle, 1, GL_FALSE, glm::value_ptr(aValue));
}

UniformVec3::UniformVec3(const Shader* const aShader, const uint64_t aLocationOverride) noexcept
: Uniform(aShader, aLocationOverride) {}
UniformVec3::UniformVec3(const Shader* const aShader, const char* aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformVec3::set(const glm::vec3& aValue) noexcept {
	glUniform3fv(this->mHandle, 1, glm::value_ptr(aValue));
}

UniformInt::UniformInt(const Shader* const aShader, const uint64_t aLocationOverride) noexcept
: Uniform(aShader, aLocationOverride) {}
UniformInt::UniformInt(const Shader* const aShader, const char* aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformInt::set(const int64_t aValue) noexcept {
	glUniform1i(this->mHandle, aValue);
}

UniformFloat::UniformFloat(const Shader* const aShader, const uint64_t aLocationOverride) noexcept
: Uniform(aShader, aLocationOverride) {}
UniformFloat::UniformFloat(const Shader* const aShader, const char* aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformFloat::set(const float aValue) noexcept {
	glUniform1f(this->mHandle, aValue);
}
