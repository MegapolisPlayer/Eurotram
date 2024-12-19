#include "Shadows.hpp"

#define LSHADOW_TEX_WIDTH

LightShadow::LightShadow(Shader& aShader, const uint64_t aWidth, const uint64_t aHeight) noexcept
	: mTextureHandle(0), mShader(aShader), mTextureWidth(aWidth), mTextureHeight(aHeight),
	mLightMatrix(1.0f), mLightMatrixLoc(&this->mShader, "uLightingShadowMatrix") {

	glGenTextures(1, &this->mTextureHandle);
	glBindTexture(GL_TEXTURE_2D, this->mTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->mTextureWidth, this->mTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	this->mFBO.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->mTextureHandle, 0);
	glDrawBuffer(GL_NONE); //ignore color data
	glReadBuffer(GL_NONE);
	this->mFBO.unbind();
}
LightShadow::LightShadow(LightShadow&& aOther) noexcept
	: mTextureHandle(aOther.mTextureHandle), mFBO(std::move(aOther.mFBO)), mShader(aOther.mShader), mLightMatrix(std::move(aOther.mLightMatrix)),
	mLightMatrixLoc(std::move(aOther.mLightMatrixLoc)) {
	aOther.mTextureHandle = 0;
}
LightShadow& LightShadow::operator=(LightShadow&& aOther) noexcept {
	this->mTextureHandle = aOther.mTextureHandle;
	this->mFBO = std::move(aOther.mFBO);
	this->mShader = std::move(aOther.mShader);
	this->mLightMatrix = std::move(aOther.mLightMatrix);
	this->mLightMatrixLoc = std::move(aOther.mLightMatrixLoc);

	aOther.mTextureHandle = 0;

	return *this;
}

void LightShadow::bind(Window& aWindow, Dirlight& aDirlight) noexcept {
	this->bindBase(aWindow);

	//direction relative to center 0,0,0
	glm::mat4 lightViewMatrix = glm::lookAt(
		glm::vec3(aDirlight.direction*300.0f),
		glm::vec3(0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	this->mLightMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 500.0f) * lightViewMatrix;

	this->mLightMatrixLoc.set(this->mLightMatrix);
}

void LightShadow::bindBase(Window& aWindow) noexcept {
	this->mShader.bind();
	aWindow.setViewport(this->mTextureWidth, this->mTextureHeight);
	this->mFBO.bind();
	glCullFace(GL_FRONT);
}

void LightShadow::unbind(Window& aWindow) noexcept {
	this->mShader.unbind();
	this->mFBO.unbind();
	aWindow.resetViewport();
	glCullFace(GL_BACK);
}

void LightShadow::bindMap(const uint64_t aSlot) noexcept {
	glActiveTexture(GL_TEXTURE0 + aSlot);
	glBindTexture(GL_TEXTURE_2D, this->mTextureHandle);
}
void LightShadow::unbindMap() noexcept {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint LightShadow::getTexture() const noexcept {
	return this->mTextureHandle;
}
glm::mat4 LightShadow::getMatrix() const noexcept {
	return this->mLightMatrix;
}

LightShadow::~LightShadow() noexcept {
	glDeleteTextures(1, &this->mTextureHandle);
}
