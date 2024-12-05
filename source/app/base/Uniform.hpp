#ifndef EUROTRAM_UNIFORM
#define EUROTRAM_UNIFORM
#include "Shader.hpp"

template<typename tType>
class Uniform {
public:
	Uniform(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	Uniform(const Shader* const aShader, const char* aUniformName) noexcept;
	virtual void set(tType aValue) noexcept = 0;
	GLint getHandle() const noexcept;
protected:
	GLint mHandle;
};

template<typename tType>
class StructUniform {
public:
	StructUniform(const uint64_t aLocation) noexcept;
	void update(const tType* const aValue) noexcept;
	void set() noexcept;
	virtual ~StructUniform() noexcept;
private:
	ShaderBuffer mBuffer;
	uint64_t mLocation;
};

class UniformMat4 : public Uniform<const glm::mat4&> {
public:
	UniformMat4(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	UniformMat4(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const glm::mat4& aValue) noexcept;
};

class UniformMat3 : public Uniform<const glm::mat3&> {
public:
	UniformMat3(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	UniformMat3(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const glm::mat3& aValue) noexcept;
};

class UniformVec3 : public Uniform<const glm::vec3&> {
public:
	UniformVec3(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	UniformVec3(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const glm::vec3& aValue) noexcept;
};
class UniformVec4 : public Uniform<const glm::vec4&> {
public:
	UniformVec4(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	UniformVec4(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const glm::vec4& aValue) noexcept;
};

class UniformInt : public Uniform<const int64_t> {
public:
	UniformInt(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	UniformInt(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const int64_t aValue) noexcept;
};

class UniformFloat : public Uniform<const float> {
public:
	UniformFloat(const Shader* const aShader, const uint64_t aLocationOverride) noexcept;
	UniformFloat(const Shader* const aShader, const char* aUniformName) noexcept;
	void set(const float aValue) noexcept;
};

#endif
