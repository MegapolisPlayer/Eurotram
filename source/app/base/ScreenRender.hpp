#ifndef EUROTRAM_SCREEN_RENDER
#define EUROTRAM_SCREEN_RENDER
#include "Texture.hpp"
#include "TextureIm.hpp"

constexpr std::string_view SCREEN_RENDER_SHADER_PATH_V = "shader/screenVertex.glsl";
constexpr std::string_view SCREEN_RENDER_SHADER_PATH_F = "shader/screenFragment.glsl";

class ScreenRenderer {
public:
	ScreenRenderer() noexcept;

	void prepareToScreen(const uint64_t aSlot, const bool aClear = true);
	void drawToScreen();

	void prepareCustomToScreen(Shader& aShader, const bool aClear = true);
	//alternative shader must have 2 vertex inputs of 2 each (2d pos, tex coord)
	void drawCustomToScreen(Shader& aShader);

	~ScreenRenderer() noexcept;
private:
	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;
	Shader mShader;
	UniformInt mTextureUniform;
};

#endif
