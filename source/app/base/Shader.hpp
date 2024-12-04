#ifndef EUROTRAM_SHADER
#define EUROTRAM_SHADER
#include "FreeUtils.hpp"

class Shader {
public:
	Shader(const std::string_view aVertexSource, const std::string_view aFragmentSource) noexcept;

	void bind() noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	~Shader();
private:
	GLuint mHandle;
};

class ShaderBuffer {
public:
	ShaderBuffer(void* const arData, const uint64_t aSizeBytes) noexcept;

	void update(void* const arData, const uint64_t aSizeBytes) noexcept;

	void bind(const uint64_t aBindLocation) noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	~ShaderBuffer() noexcept;
private:
	GLuint mHandle;
};

#endif
