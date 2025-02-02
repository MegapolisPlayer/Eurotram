#ifndef EUROTRAM_UNIFORM
#define EUROTRAM_UNIFORM
#include "Buffer.hpp"
#include "Shader.hpp"

template<typename tType>
class Uniform {
public:
	Uniform(const uint64_t aLocationOverride) noexcept;
	Uniform(const Shader& aShader, const std::string_view aUniformName) noexcept;

	virtual void set(tType aValue) noexcept = 0;
	GLint getHandle() const noexcept;
protected:
	GLint mHandle = 0;
};

//TODO make set take args and rename update to bind()

template<typename tType>
class StructUniform {
public:
	StructUniform(const uint64_t aLocation, const uint64_t aAmount) noexcept;

	//resizes buffer, use only if size change required
	void setNewData(const tType* const aValue, const uint64_t aAmount);
	//last element NON inclusive
	void update(const tType* const aValue, const uint64_t aFirstElem = 0, const uint64_t aLastElem = 0) noexcept;

	void set() noexcept;
	virtual ~StructUniform() noexcept;
private:
	ShaderBuffer mBuffer;
	uint64_t mLocation;
	uint64_t mAmount;
};

class UniformMat4 : public Uniform<const glm::mat4&> {
public:
	UniformMat4(const uint64_t aLocationOverride) noexcept;
	UniformMat4(const Shader& aShader, const std::string_view aUniformName) noexcept;
	void set(const glm::mat4& aValue) noexcept;
};

class UniformMat3 : public Uniform<const glm::mat3&> {
public:
	UniformMat3(const uint64_t aLocationOverride) noexcept;
	UniformMat3(const Shader& aShader, const std::string_view aUniformName) noexcept;
	void set(const glm::mat3& aValue) noexcept;
};

class UniformVec3 : public Uniform<const glm::vec3&> {
public:
	UniformVec3(const uint64_t aLocationOverride) noexcept;
	UniformVec3(const Shader& aShader, const std::string_view aUniformName) noexcept;
	void set(const glm::vec3& aValue) noexcept;
};
class UniformVec4 : public Uniform<const glm::vec4&> {
public:
	UniformVec4(const uint64_t aLocationOverride) noexcept;
	UniformVec4(const Shader& aShader, const std::string_view aUniformName) noexcept;
	void set(const glm::vec4& aValue) noexcept;
};

class UniformInt : public Uniform<const int64_t> {
public:
	UniformInt(const uint64_t aLocationOverride) noexcept;
	UniformInt(const Shader& aShader, const std::string_view aUniformName) noexcept;
	void set(const int64_t aValue) noexcept;
};

template <ContiguousStandardNotationContainer tType>
class UniformIntMultiple : public Uniform<tType> {
public:
	UniformIntMultiple(const uint64_t aLocationOverride) noexcept;
	UniformIntMultiple(const Shader& aShader, const std::string_view aUniformName) noexcept;

	void set(tType aValue) noexcept;
};

class UniformFloat : public Uniform<const float> {
public:
	UniformFloat(const uint64_t aLocationOverride) noexcept;
	UniformFloat(const Shader& aShader, const std::string_view aUniformName) noexcept;
	void set(const float aValue) noexcept;
};

#endif
