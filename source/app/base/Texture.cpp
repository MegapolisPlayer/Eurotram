#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"

Texture::Texture(const char* aFilename, const uint64_t aId) noexcept
	: mpData(nullptr) {
	glGenTextures(1, &this->mHandle);
	glBindTexture(GL_TEXTURE_2D, this->mHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	this->mpData = stbi_load(aFilename, &this->mWidth, &this->mHeight, &this->mChannels, 4);
	if(!this->mpData) {
		std::cerr << LogLevel::ERROR << "STBI failed to load image!\n" << LogLevel::RESET;
		return;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->mWidth, this->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->mpData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, this->mHandle + aId);
}

void Texture::bind(const uint64_t aId) noexcept {
	glActiveTexture(GL_TEXTURE0 + GL_TEXTURE0 + aId);
	glBindTexture(GL_TEXTURE_2D, this->mHandle);
}
void Texture::unbind() noexcept {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getHandle() const noexcept {
	return this->mHandle;
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
