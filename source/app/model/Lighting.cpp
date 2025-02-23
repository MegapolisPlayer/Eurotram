#include "Lighting.hpp"

void setSpotlightRadius(Spotlight* aSpotlight, const float aRadius, const float aCutoffRange) noexcept {
	aSpotlight->radius = glm::cos(glm::radians(aRadius));
	aSpotlight->inner = glm::cos(glm::radians(aRadius-(aCutoffRange/2.0)));
	aSpotlight->outer = glm::cos(glm::radians(aRadius+(aCutoffRange/2.0)));
}

void setAttenuation(const AttenuationValues aDistance, float* aConstant, float* aLinear, float* aQuadratic) noexcept {
	*aConstant = 1.0;
	switch(aDistance) {
		case AttenuationValues::DISTANCE_7:
			*aLinear = 0.7;
			*aQuadratic = 1.8;
			return;
		case AttenuationValues::DISTANCE_20:
			*aLinear = 0.22;
			*aQuadratic = 0.20;
			return;
		case AttenuationValues::DISTANCE_50:
			*aLinear = 0.09;
			*aQuadratic = 0.032;
			return;
		case AttenuationValues::DISTANCE_100:
			*aLinear = 0.045;
			*aQuadratic = 0.0075;
			return;
		case AttenuationValues::DISTANCE_200:
			*aLinear = 0.022;
			*aQuadratic = 0.0019;
			return;
	}
}

UniformDirlight::UniformDirlight(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: StructUniform(aLocation, aAmount) {}
UniformDirlight::~UniformDirlight() noexcept {}

UniformPointlight::UniformPointlight(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: StructUniform(aLocation, aAmount) {}
UniformPointlight::~UniformPointlight() noexcept {}

UniformSpotlight::UniformSpotlight(const uint64_t aLocation, const uint64_t aAmount) noexcept
	: StructUniform(aLocation, aAmount) {}
UniformSpotlight::~UniformSpotlight() noexcept {}

DirectionalShadows::DirectionalShadows(
	const glm::vec3& aPos, const glm::vec3& aSceneCenter,
	const float aProjectionSize, const float aNear, const float aFar
) noexcept
	: mFBO(), mTexture(
		DirectionalShadowMapWidth, DirectionalShadowMapHeight,
		GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
		TextureScale::NEAREST_NEIGHBOR, TextureBorder::FILL_OUT_OF_RANGE
	) {
	this->mTexture.setOutOfBoundsColor(1.0, 1.0, 1.0); //out of bounds - no shadow
	this->mFBO.bind();
	this->mFBO.bindTexture(this->mTexture, GL_DEPTH_ATTACHMENT);
	this->mView = glm::lookAt(aPos, aSceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	this->mProjection = glm::ortho(-aProjectionSize/2.0f, aProjectionSize/2.0f, -aProjectionSize/2.0f, aProjectionSize/2.0f, aNear, aFar);
	this->mLightMatrix = this->mProjection * this->mView;
	this->mFBO.unbind();
}
DirectionalShadows::DirectionalShadows(DirectionalShadows&& aOther) noexcept
	: mFBO(std::move(aOther.mFBO)), mTexture(std::move(aOther.mTexture)) {
	this->mView = std::move(aOther.mView);
	this->mProjection = std::move(aOther.mProjection);
	this->mLightMatrix = std::move(aOther.mLightMatrix);
}
DirectionalShadows& DirectionalShadows::operator=(DirectionalShadows&& aOther) noexcept {
	this->mFBO = std::move(aOther.mFBO);
	this->mTexture = std::move(aOther.mTexture);
	this->mView = std::move(aOther.mView);
	this->mProjection = std::move(aOther.mProjection);
	this->mLightMatrix = std::move(aOther.mLightMatrix);
	return *this;
}

void DirectionalShadows::beginPass(Window& aWindow, UniformMat4& aLightMatrixUniform) noexcept {
	aWindow.setViewport(DirectionalShadowMapWidth, DirectionalShadowMapHeight);
	this->mFBO.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	aLightMatrixUniform.set(this->mLightMatrix);
}
void DirectionalShadows::endPass(Window& aWindow) noexcept {
	glCullFace(GL_BACK);
	this->mFBO.unbind();
	aWindow.resetViewport();
}

void DirectionalShadows::setPosDirection(const glm::vec3& aPos, const glm::vec3& aDirection) noexcept {
	this->mView = glm::lookAt(aPos, aPos + aDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	this->mLightMatrix = this->mProjection * this->mView;
}
void DirectionalShadows::setPosCenter(const glm::vec3& aPos, const glm::vec3& aSceneCenter) noexcept {
	this->mView = glm::lookAt(aPos, aSceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	this->mLightMatrix = this->mProjection * this->mView;
}
void DirectionalShadows::setProjection(const float aProjectionSize, const float aNear, const float aFar) noexcept {
	this->mProjection = glm::ortho(-aProjectionSize/2.0f, aProjectionSize/2.0f, -aProjectionSize/2.0f, aProjectionSize/2.0f, aNear, aFar);
	this->mLightMatrix = this->mProjection * this->mView;
}

void DirectionalShadows::bindMap(const uint64_t aTextureSlot) noexcept {
	this->mTexture.bind(aTextureSlot);
}

const glm::mat4& DirectionalShadows::getProjectionMatrix() const noexcept {
	return this->mLightMatrix;
}
Texture& DirectionalShadows::getFBOTexture() noexcept {
	return this->mTexture;
}

DirectionalShadows::~DirectionalShadows() noexcept {}

SpotlightShadows::SpotlightShadows(
	const glm::vec3& aPos, const glm::vec3& aDirection,
	const float aNear, const float aFar, const float aFOV
) noexcept : mFBO(), mTexture(
	DirectionalShadowMapWidth, DirectionalShadowMapHeight,
	GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
	TextureScale::NEAREST_NEIGHBOR, TextureBorder::FILL_OUT_OF_RANGE
) {
	this->mTexture.setOutOfBoundsColor(1.0, 1.0, 1.0); //out of bounds - no shadow
	this->mFBO.bind();
	this->mFBO.bindTexture(this->mTexture, GL_DEPTH_ATTACHMENT);
	this->mProjection = glm::perspective(glm::radians(aFOV), (float)StandardShadowMapWidth/StandardShadowMapHeight, aNear, aFar);
	this->mView = glm::lookAt(aPos, aPos + aDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	this->mLightMatrix = this->mProjection * this->mView;
	this->mFBO.unbind();
}
SpotlightShadows::SpotlightShadows(SpotlightShadows&& aOther) noexcept
	: mFBO(std::move(aOther.mFBO)), mTexture(std::move(aOther.mTexture)) {
	this->mView = std::move(aOther.mView);
	this->mProjection = std::move(aOther.mProjection);
	this->mLightMatrix = std::move(aOther.mLightMatrix);
}
SpotlightShadows& SpotlightShadows::operator=(SpotlightShadows&& aOther) noexcept {
	this->mFBO = std::move(aOther.mFBO);
	this->mTexture = std::move(aOther.mTexture);
	this->mView = std::move(aOther.mView);
	this->mProjection = std::move(aOther.mProjection);
	this->mLightMatrix = std::move(aOther.mLightMatrix);
	return *this;
}

void SpotlightShadows::beginPass(Window& aWindow, UniformMat4& aLightMatrixUniform) noexcept {
	aWindow.setViewport(StandardShadowMapWidth, StandardShadowMapHeight);
	this->mFBO.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	aLightMatrixUniform.set(this->mLightMatrix);
}
void SpotlightShadows::endPass(Window& aWindow) noexcept {
	glCullFace(GL_BACK);
	this->mFBO.unbind();
	aWindow.resetViewport();
}

void SpotlightShadows::setPosDirection(const glm::vec3& aPos, const glm::vec3& aDirection) noexcept {
	this->mView = glm::lookAt(aPos, aPos + aDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	this->mLightMatrix = this->mProjection * this->mView;
}
void SpotlightShadows::setPosCenter(const glm::vec3& aPos, const glm::vec3& aSceneCenter) noexcept {
	this->mView = glm::lookAt(aPos, aSceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	this->mLightMatrix = this->mProjection * this->mView;
}
void SpotlightShadows::setProjection(const float aNear, const float aFar, const float aFOV) noexcept {
	this->mProjection = glm::perspective(glm::radians(aFOV), (float)StandardShadowMapWidth/StandardShadowMapHeight, aNear, aFar);
	this->mLightMatrix = this->mProjection * this->mView;
}

void SpotlightShadows::bindMap(const uint64_t aTextureSlot) noexcept {
	this->mTexture.bind(aTextureSlot);
}

const glm::mat4& SpotlightShadows::getProjectionMatrix() const noexcept {
	return this->mLightMatrix;
}
Texture& SpotlightShadows::getFBOTexture() noexcept {
	return this->mTexture;
}

SpotlightShadows::~SpotlightShadows() noexcept {}
