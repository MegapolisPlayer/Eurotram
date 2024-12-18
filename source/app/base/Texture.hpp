#ifndef EUROTRAM_TEXTURE
#define EUROTRAM_TEXTURE
#include "Uniform.hpp"
#include "UniformIm.hpp"

//TODO add option to generate blank texture

class Texture {
public:
	Texture(std::string_view aFilename) noexcept;
	Texture(Texture&& aOther) noexcept;
	Texture& operator=(Texture&& aOther) noexcept;
	Texture(Texture& aOther) noexcept = delete;
	Texture& operator=(Texture& aOther) noexcept = delete;

	void bind(const uint64_t aId) noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;
	std::string_view getPath() const noexcept;

	static uint64_t getAmountOfSlots() noexcept;

	~Texture() noexcept;
private:
	std::string_view mPath; //so no duplicates
	GLuint mHandle;
	GLubyte* mpData;
	int32_t mWidth, mHeight, mChannels;
};

#endif
