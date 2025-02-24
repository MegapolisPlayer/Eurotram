#ifndef EUROTRAM_ORDER_INDEPENDENT
#define EUROTRAM_ORDER_INDEPENDENT
#include "Buffer.hpp"
#include "Texture.hpp"
#include "TextureIm.hpp"
#include "Window.hpp"
#include "ScreenRender.hpp"

constexpr std::string_view OIT_SHADER_PATH_V = "shader/screenVertex.glsl"; //reuse from screen
constexpr std::string_view OIT_SHADER_PATH_F = "shader/OITFragment.glsl";

//handles order independent transparency
class OIT {
public:
	OIT(Window& aWindow) noexcept;

	//uniform which will discard transparency (1), solids (2) or nothing (0)
	void beginOpaquePass(Window& aWindow, UniformInt& aEnabledUniform) noexcept;
	void endOpaquePass(UniformInt& aEnabledUniform) noexcept;

	void beginTransparentPass(UniformInt& aEnabledUniform) noexcept;
	void endTransparentPass(UniformInt& aEnabledUniform) noexcept;

	void draw(Window& aWindow, ScreenRenderer& aSR) noexcept;

	~OIT() noexcept;
private:
	Framebuffer mOpaqueFBO;
	Framebuffer mTransparentFBO;
	//multisampled textures for anti aliasing
	FramebufferMultisampleTexture mOpaque;
	FramebufferMultisampleTexture mAccum;
	FramebufferMultisampleTexture mReveal;
	FramebufferMultisampleTexture mDepth;
	Shader mShader;

	Framebuffer mDestinationFBO;
	Texture mDestTexture;
};

#endif
