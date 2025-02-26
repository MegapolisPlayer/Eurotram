#include "OIT.hpp"

//Weighted-blended OIT
//https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
//computing coverage and color - then combining

//framebuffers complete, screen rendering works!

glm::vec4 ZERO_FILLER = glm::vec4(0.0);
glm::vec4 ONE_FILLER = glm::vec4(1.0);

//TODO add resize support

OIT::OIT(Window& aWindow) noexcept :
	mOpaque(aWindow.getWidth(), aWindow.getHeight(), GL_RGBA16F),
	mAccum(aWindow.getWidth(), aWindow.getHeight(), GL_RGBA16F),
	mReveal(aWindow.getWidth(), aWindow.getHeight(), GL_R8),
	mDepth(aWindow.getWidth(), aWindow.getHeight(), GL_DEPTH_COMPONENT),
	mShader(OIT_SHADER_PATH_V, OIT_SHADER_PATH_F),
	mDestTexture(aWindow.getWidth(), aWindow.getHeight(), GL_RGBA8, GL_RGBA, GL_FLOAT, true) {
	this->mOpaqueFBO.bindTexture(this->mOpaque,  GL_COLOR_ATTACHMENT0, true, true);
	this->mOpaqueFBO.bindTexture(this->mDepth, GL_DEPTH_ATTACHMENT, true, true);
	this->mTransparentFBO.bindTexture(this->mAccum, GL_COLOR_ATTACHMENT0, true, true);
	this->mTransparentFBO.bindTexture(this->mReveal, GL_COLOR_ATTACHMENT1, true, true);
	this->mTransparentFBO.bindTexture(this->mDepth, GL_DEPTH_ATTACHMENT, true, true);
	this->mDestinationFBO.bindTexture(this->mDestTexture, GL_COLOR_ATTACHMENT0, true, true);

	this->mTransparentFBO.bind();
	const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, transparentDrawBuffers);

	this->mOpaqueFBO.checkStatus();
	this->mTransparentFBO.checkStatus();

	this->mOpaqueFBO.unbind();
	this->mTransparentFBO.unbind();
}


void OIT::beginOpaquePass(Window& aWindow, UniformInt& aEnabledUniform) noexcept {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	this->mOpaqueFBO.bind();
	aWindow.applyBackgroundColor();
	aEnabledUniform.set(1); //discard transparent
}
void OIT::endOpaquePass(UniformInt& aEnabledUniform) noexcept {
	//reset to default
	glEnable(GL_BLEND);
	this->mOpaqueFBO.unbind();
	aEnabledUniform.set(0);
}

void OIT::beginTransparentPass(UniformInt& aEnabledUniform) noexcept {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE); //accumulation blend target
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR); //reveal blend target
	glBlendEquation(GL_FUNC_ADD);
	this->mTransparentFBO.bind();
	glClearBufferfv(GL_COLOR, 0, (float*)&ZERO_FILLER);
	glClearBufferfv(GL_COLOR, 1, (float*)&ONE_FILLER);
	aEnabledUniform.set(2); //discard solid
}
void OIT::endTransparentPass(UniformInt& aEnabledUniform) noexcept {
	//reset to default
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->mTransparentFBO.unbind();
	aEnabledUniform.set(0);
}

void OIT::draw(Window& aWindow, ScreenRenderer& aSR) noexcept {
	//composite

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->mOpaqueFBO.bind(); //with opaque texture

	aSR.prepareCustomToScreen(this->mShader, false);
	this->mAccum.bind(0);
	this->mReveal.bind(1);
	aSR.drawCustomToScreen(this->mShader);

	//copy and apply MSAA

	this->mDestinationFBO.bind();
	glBlitNamedFramebuffer(
		this->mOpaqueFBO.getHandle(), this->mDestinationFBO.getHandle(),
		0, 0, aWindow.getWidth(), aWindow.getHeight(), 0, 0, aWindow.getWidth(), aWindow.getHeight(),
		GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST
	);

	//draw

	glDisable(GL_DEPTH_TEST); //removes from pipeline
	glDepthMask(GL_TRUE); //condition
	glDisable(GL_BLEND);

	this->mOpaqueFBO.unbind();
	this->mTransparentFBO.unbind();
	this->mDestinationFBO.unbind();

	//draw to backbuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	aSR.prepareToScreen(0);
	this->mDestTexture.bind(0);
	aSR.drawToScreen();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //culprit of shadow issues
}

OIT::~OIT() noexcept {

}
