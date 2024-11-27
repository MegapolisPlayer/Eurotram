#ifndef EUROTRAM_WINDOW
#define EUROTRAM_WINDOW
#include "FreeUtils.hpp"

class Window;

//X coord, Y coord relative to top-left corner of window
typedef void(*WindowMouseCallback)(Window*, double, double);
//GLFW key, action and modifier bits
typedef void(*WindowActionCallback)(Window*, uint32_t, uint32_t, uint32_t);

class Window {
public:
	Window(const char* aTitle, uint64_t aWidth, const uint64_t aHeight, const bool aFullscreen = false, const bool aDebug = false) noexcept;

	void bindContext() noexcept;

	bool isOpen() const noexcept;

	void beginFrame() noexcept;
	void endFrame() noexcept;

	void hideCursor() noexcept;
	void showCursor() noexcept;

	void registerKeyCallback(const uint32_t aGLFWKey, WindowActionCallback aCallback) noexcept;
	uint32_t registerClickCallback(WindowActionCallback aCallback) noexcept;
	uint32_t registerGenericKeyCallback(WindowActionCallback aCallback) noexcept;
	uint32_t registerMouseCallback(WindowMouseCallback aCallback) noexcept;

	void removeKeyCallback(const uint32_t aGLFWKey) noexcept;
	void removeClickCallback(const uint32_t aId) noexcept;
	void removeGenericKeyCallback(const uint32_t aId) noexcept;
	void removeMouseCallback(const uint32_t aId) noexcept;

	//only closes the window - doesn't destroy it
	void close() noexcept;

	bool isCursorHidden() const noexcept;

	~Window() noexcept;
private:
	GLFWwindow* mpHandle;

	bool mDebugEnabled;
	bool mExitOverride;
	bool mCursorHidden;

	std::array<float, 4> mBackgroundColor;
	std::vector<std::pair<uint32_t, WindowActionCallback>> mKeyCallbacks;
	std::vector<WindowActionCallback> mGenericKeyCallbacks;
	std::vector<WindowActionCallback> mClickCallbacks;
	std::vector<WindowMouseCallback> mMouseCallbacks;

	static void MouseCallback(GLFWwindow* aWindow, double aX, double aY) noexcept;
	static void KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aModifiers) noexcept;
	static void ClickCallback(GLFWwindow* aWindow, int aKey, int aAction, int aModifiers) noexcept;
	static void GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) noexcept;
};

#endif
