#ifndef EUROTRAM_WINDOW
#define EUROTRAM_WINDOW
#include "Setup.hpp"

class Window;

//X coord, Y coord relative to top-left corner of window
typedef void(*WindowMouseCallback)(Window*, double, double);
//GLFW key, action and modifier bits
typedef void(*WindowKeyCallback)(Window*, uint32_t, uint32_t, uint32_t);

class Window {
public:
	Window(const char* aTitle, uint64_t aWidth, const uint64_t aHeight, const bool aFullscreen = false, const bool aDebug = false) noexcept;

	//binds window context - context is automatically bound after constructor
	void bindContext() noexcept;
	void bindDebugContext() noexcept;

	//returns if window still open
	bool isOpen() const noexcept;

	//clears window background
	void clear() noexcept;
	//updates window
	void update() noexcept;

	//hides the cursor
	void hideCursor() noexcept;
	//shows the normal cursor
	void showCursor() noexcept;

	//registers key callback
	void registerKeyCallback(const uint32_t aGLFWKey, WindowKeyCallback aCallback) noexcept;
	//registers key callback for multiple keys
	uint32_t registerGenericKeyCallback(WindowKeyCallback aCallback) noexcept;
	//registers mouse callback - returns ID
	uint32_t registerMouseCallback(WindowMouseCallback aCallback) noexcept;
	//remove key callback
	void removeKeyCallback(const uint32_t aGLFWKey) noexcept;
	//remove generic key callback
	void removeGenericKeyCallback(const uint32_t aId) noexcept;
	//remove mouse callback
	void removeMouseCallback(const uint32_t aId) noexcept;

	//only closes the window - doesn't destroy it
	void close() noexcept;

	~Window() noexcept;
private:
	GLFWwindow* mpHandle;

	bool mDebugEnabled;
	bool mExitOverride;
	std::array<uint64_t, 4> mBackgroundColor;
	std::vector<std::pair<uint32_t, WindowKeyCallback>> mKeyCallbacks;
	std::vector<WindowKeyCallback> mGenericKeyCallbacks;
	std::vector<WindowMouseCallback> mMouseCallbacks;

	static void MouseCallback(GLFWwindow* aWindow, double aX, double aY);
	static void KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aModifiers);
	static void GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};

#endif
