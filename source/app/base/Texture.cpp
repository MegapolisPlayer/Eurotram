#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"
#include "TextureIm.hpp"

Texture::Texture(std::string_view aFilename) noexcept
	: mHandle(0), mpData(nullptr), mPath(aFilename), mWidth(0), mHeight(0), mChannels(0) {
	if(this->mPath.empty()) {
		return;
	}

	glGenTextures(1, &this->mHandle);
	glBindTexture(GL_TEXTURE_2D, this->mHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	this->mpData = stbi_load(mPath.data(), &this->mWidth, &this->mHeight, &this->mChannels, 4);
	if(!this->mpData) {
		std::cerr << LogLevel::ERROR << "STBI failed to load image " << aFilename << "!\n" << LogLevel::RESET;
		return;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->mWidth, this->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->mpData);
	glGenerateMipmap(GL_TEXTURE_2D);
}
Texture::Texture(Texture&& aOther) noexcept
	: mPath(std::move(aOther.mPath)), mHandle(aOther.mHandle), mpData(aOther.mpData),
	mWidth(aOther.mWidth), mHeight(aOther.mHeight), mChannels(aOther.mChannels) {
	aOther.mHandle = 0;
	aOther.mpData = nullptr;
}
Texture& Texture::operator=(Texture&& aOther) noexcept {
	this->mPath = std::move(aOther.mPath);
	this->mHandle = aOther.mHandle;
	this->mpData = aOther.mpData;
	this->mWidth = aOther.mWidth;
	this->mHeight = aOther.mHeight;
	this->mChannels = aOther.mChannels;

	aOther.mHandle = 0;
	aOther.mpData = nullptr;
	return *this;
}

void Texture::bind(const uint64_t aId) noexcept {
	if(this->mHandle == 0) return;
	glActiveTexture(GL_TEXTURE0 + aId);
	glBindTexture(GL_TEXTURE_2D, this->mHandle);
}
void Texture::unbind() noexcept {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getHandle() const noexcept {
	return this->mHandle;
}
std::string_view Texture::getPath() const noexcept {
	return this->mPath;
}

uint64_t Texture::getAmountOfSlots() noexcept {
	GLint amount;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &amount);
	return amount;
}

Texture::~Texture() noexcept {
	glDeleteTextures(1, &this->mHandle);
	stbi_image_free(this->mpData);
}
