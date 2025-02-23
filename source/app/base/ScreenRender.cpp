#include "ScreenRender.hpp"

static GLfloat SR_VERTICES[] = {
	-1.0f, -1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
};
static GLuint SR_INDICES[] = {
	0, 1, 2, 2, 3, 0
};

ScreenRenderer::ScreenRenderer() noexcept
	: mVAO(), mVBO(SR_VERTICES, 4, 4), mIBO(SR_INDICES, 6),
	mShader(SCREEN_RENDER_SHADER_PATH_V, SCREEN_RENDER_SHADER_PATH_F), mTextureUniform(50) {
	this->mVBO.enableAttribute(&this->mVAO, 2); //pos
	this->mVBO.enableAttribute(&this->mVAO, 2); //texcoord
}
void ScreenRenderer::drawToScreen(const uint64_t aSlot, const bool aClear) {
	glDisable(GL_DEPTH_TEST);
	if(aClear) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	this->mVAO.bind();
	this->mShader.bind();
	this->mVBO.bind();
	this->mIBO.bind();
	this->mTextureUniform.set(aSlot);
	this->mIBO.draw();
	this->mShader.unbind();
	this->mVAO.unbind();
	glEnable(GL_DEPTH_TEST);
}
void ScreenRenderer::drawCustomToScreen(Shader& aShader, const bool aClear) {
	glDisable(GL_DEPTH_TEST);
	if(aClear) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	this->mVAO.bind();
	aShader.bind();
	this->mVBO.bind();
	this->mIBO.bind();
	this->mIBO.draw();
	aShader.unbind();
	this->mVAO.unbind();
	glEnable(GL_DEPTH_TEST);
}
ScreenRenderer::~ScreenRenderer() noexcept {}
