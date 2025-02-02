#include "Uniform.hpp"
#include "UniformIm.hpp"

UniformMat4::UniformMat4(const uint64_t aLocationOverride) noexcept
	: Uniform(aLocationOverride) {}
UniformMat4::UniformMat4(const Shader& aShader, const std::string_view aUniformName) noexcept
	: Uniform(aShader, aUniformName) {}
void UniformMat4::set(const glm::mat4& aValue) noexcept {
	glUniformMatrix4fv(this->mHandle, 1, GL_FALSE, glm::value_ptr(aValue));
}

UniformMat3::UniformMat3(const uint64_t aLocationOverride) noexcept
: Uniform(aLocationOverride) {}
UniformMat3::UniformMat3(const Shader& aShader, const std::string_view aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformMat3::set(const glm::mat3& aValue) noexcept {
	glUniformMatrix3fv(this->mHandle, 1, GL_FALSE, glm::value_ptr(aValue));
}

UniformVec3::UniformVec3(const uint64_t aLocationOverride) noexcept
: Uniform(aLocationOverride) {}
UniformVec3::UniformVec3(const Shader& aShader, const std::string_view aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformVec3::set(const glm::vec3& aValue) noexcept {
	glUniform3fv(this->mHandle, 1, glm::value_ptr(aValue));
}

UniformVec4::UniformVec4(const uint64_t aLocationOverride) noexcept
: Uniform(aLocationOverride) {}
UniformVec4::UniformVec4(const Shader& aShader, const std::string_view aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformVec4::set(const glm::vec4& aValue) noexcept {
	glUniform4fv(this->mHandle, 1, glm::value_ptr(aValue));
}

UniformInt::UniformInt(const uint64_t aLocationOverride) noexcept
: Uniform(aLocationOverride) {}
UniformInt::UniformInt(const Shader& aShader, const std::string_view aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformInt::set(const int64_t aValue) noexcept {
	glUniform1i(this->mHandle, aValue);
}

UniformFloat::UniformFloat(const uint64_t aLocationOverride) noexcept
: Uniform(aLocationOverride) {}
UniformFloat::UniformFloat(const Shader& aShader, const std::string_view aUniformName) noexcept
: Uniform(aShader, aUniformName) {}
void UniformFloat::set(const float aValue) noexcept {
	glUniform1f(this->mHandle, aValue);
}
