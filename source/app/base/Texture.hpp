#ifndef EUROTRAM_TEXTURE
#define EUROTRAM_TEXTURE
#include "Uniform.hpp"
#include "UniformIm.hpp"

class Texture {
public:
	//channels = bits per pixel
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

//TODO implement abstractions for stbi image write

template<uint8_t tAmount = 32>
class TextureSamplerArray {
public:
	TextureSamplerArray(const Shader& aShader, const uint64_t aSamplerArrayLocation) noexcept;
	TextureSamplerArray(const Shader& aShader, const std::string_view aSamplerArrayName) noexcept;

	void set() noexcept;

	UniformIntMultiple<std::array<GLint, tAmount>>& getUniform() noexcept;

	~TextureSamplerArray() noexcept;
private:
	UniformIntMultiple<std::array<GLint, tAmount>&> mUniform;
	std::array<GLint, tAmount> mArray;
};

#endif
