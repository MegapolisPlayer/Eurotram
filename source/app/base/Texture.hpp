#ifndef EUROTRAM_TEXTURE
#define EUROTRAM_TEXTURE
#include "Uniform.hpp"
#include "UniformIm.hpp"

class Texture {
public:
	Texture(const char* aFilename, const uint64_t aId = 0) noexcept;

	void bind(const uint64_t aId) noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	static uint64_t getAmountOfSlots() noexcept;

	~Texture() noexcept;
private:
	GLuint mHandle;
	GLubyte* mpData;
	int32_t mWidth, mHeight, mChannels;
};

#endif
