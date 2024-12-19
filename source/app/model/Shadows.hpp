#ifndef EUROTRAM_MODEL_SHADOWS
#define EUROTRAM_MODEL_SHADOWS
#include "Lighting.hpp"

//We only need shadows for directional spotlights
//Pointlights - only used in internal tram lighting or blinkers
//This will save performance

class LightShadow {
public:
	//shader MUST have uLightingShadowMatrix uniform
	LightShadow(Shader& aShader, const uint64_t aWidth = 2048, const uint64_t aHeight = 2048) noexcept; //depth processing shader
	LightShadow(LightShadow&& aOther) noexcept;
	LightShadow& operator=(LightShadow&& aOther) noexcept;
	LightShadow(LightShadow& aOther) noexcept = delete;
	LightShadow& operator=(LightShadow& aOther) noexcept = delete;

	void bind(Window& aWindow, Dirlight& aDirlight) noexcept;
	void unbind(Window& aWindow) noexcept;

	//binds map as texture
	void bindMap(const uint64_t aSlot) noexcept;
	void unbindMap() noexcept;

	GLuint getTexture() const noexcept;
	glm::mat4 getMatrix() const noexcept;

	~LightShadow() noexcept;
private:
	Framebuffer mFBO;
	GLuint mTextureHandle;
	uint64_t mTextureWidth, mTextureHeight;
	Shader& mShader;

	UniformMat4 mLightMatrixLoc;
	glm::mat4 mLightMatrix;

	void bindBase(Window& aWindow) noexcept;
};

#endif
