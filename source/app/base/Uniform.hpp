#ifndef EUROTRAM_UNIFORM
#define EUROTRAM_UNIFORM
#include "Shader.hpp"

template<typename tType>
class Uniform {
public:
	Uniform(const Shader* const aShader, const char* aUniformName) noexcept;
	virtual void set(tType aValue) noexcept = 0;
	GLint getHandle() const noexcept;
protected:
	GLint mHandle;
};

class UniformMat4 : public Uniform<const glm::mat4&> {
public:
	UniformMat4(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const glm::mat4& aValue) noexcept;
};

#endif
