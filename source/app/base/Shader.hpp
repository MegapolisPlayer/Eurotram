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
	ShaderBuffer(const void* const arData, const uint64_t aSizeBytes) noexcept;

	void update(const void* const arData, const uint64_t aSizeBytes, const uint64_t aOffset = 0) noexcept;

	void bind(const uint64_t aBindLocation) noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	~ShaderBuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mSizeBytes;
};

#endif
