#include "OIT.hpp"

//Weighted-blended OIT
//https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
//computing coverage and color - then combining

//TODO fix
//ACCUM texture doesnt work
//REVEAL does show something

//framebuffers complete, screen rendering works!

glm::vec4 ZERO_FILLER = glm::vec4(0.0);
glm::vec4 ONE_FILLER = glm::vec4(1.0);

OIT::OIT(Window& aWindow) noexcept
	: mAccum(aWindow.getWidth(), aWindow.getHeight(), GL_RGBA16F, GL_RGBA), mReveal(aWindow.getWidth(), aWindow.getHeight(), GL_R8, GL_RED),
	mShader(OIT_SHADER_PATH_V, OIT_SHADER_PATH_F) {
		this->mTransparent.bind();
		this->mOpaque.bindTexture(this->mAccum,  GL_COLOR_ATTACHMENT0);
		this->mTransparent.bindTexture(this->mReveal, GL_COLOR_ATTACHMENT1);
		const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, transparentDrawBuffers);

		this->mOpaque.checkStatus();
		this->mTransparent.checkStatus();

		this->mOpaque.unbind();
		this->mTransparent.unbind();
	}

void OIT::beginOpaquePass(UniformInt& aEnabledUniform) noexcept {
	glEnable(GL_DEPTH_TEST); //CHECKED
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	aEnabledUniform.set(1); //discard transparent
	this->mOpaque.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OIT::endOpaquePass(UniformInt& aEnabledUniform) noexcept {
	this->mOpaque.unbind();
	aEnabledUniform.set(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
}

void OIT::beginTransparentPass(UniformInt& aEnabledUniform) noexcept {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE); //accumulation blend target
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR); //reveal blend target
	glBlendEquation(GL_FUNC_ADD);
	aEnabledUniform.set(2); //discard solid
	this->mTransparent.bind();
	glClearBufferfv(GL_COLOR, 0, (float*)&ZERO_FILLER);
	glClearBufferfv(GL_COLOR, 1, (float*)&ONE_FILLER);
}
void OIT::endTransparentPass(UniformInt& aEnabledUniform) noexcept {
	this->mTransparent.unbind();
	aEnabledUniform.set(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
}

void OIT::draw(ScreenRenderer& aSR) noexcept {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->mOpaque.unbind();
	this->mTransparent.unbind();
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	this->mAccum.bind(0);
	this->mReveal.bind(1);
	aSR.drawCustomToScreen(this->mShader, false);
}

OIT::~OIT() noexcept {

}
