#include "Uniform.hpp"
#include "UniformIm.hpp"

UniformMat4::UniformMat4(const Shader* const aShader, const char* aUniformName) noexcept
	: Uniform(aShader, aUniformName) {}
void UniformMat4::set(const glm::mat4& aValue) noexcept {
	glUniformMatrix4fv(this->mHandle, 1, GL_FALSE, glm::value_ptr(aValue));
}
