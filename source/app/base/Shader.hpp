#ifndef EUROTRAM_SHADER
#define EUROTRAM_SHADER
#include "FreeUtils.hpp"

class Shader {
public:
	Shader(const std::string_view aVertexSource, const std::string_view aFragmentSource) noexcept;
	Shader(Shader&& aOther) noexcept;
	Shader& operator=(Shader&& aOther) noexcept;
	Shader(Shader& aOther) noexcept = delete;
	Shader& operator=(Shader& aOther) noexcept = delete;

	void bind() noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	~Shader();
private:
	GLuint mHandle;
};

#endif
