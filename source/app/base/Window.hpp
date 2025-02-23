#ifndef EUROTRAM_WINDOW
#define EUROTRAM_WINDOW
#include "FreeUtils.hpp"

class Camera;
class Window;

//X coord, Y coord relative to top-left corner of window
typedef void(*WindowMouseCallback)(Window*, double, double);
//GLFW key, action and modifier bits
typedef void(*WindowActionCallback)(Window*, uint32_t, uint32_t, uint32_t);

class Window {
	friend class Camera;
public:
	Window(const char* aTitle, uint64_t aWidth, const uint64_t aHeight, const bool aFullscreen = false, const bool aDebug = false) noexcept;

	void enableVSync() noexcept;
	void disableVSync() noexcept;

	void setBackgroundColor(const std::array<float, 4>& aArray) noexcept;
	void setBackgroundColor(const glm::vec4& aArray) noexcept;

	void setViewport(const uint32_t aWidth, const uint32_t aHeight) noexcept;
	void resetViewport() noexcept; //to window size

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

	void bindCamera(Camera* const aCamera) noexcept;
	Camera* getCamera() const noexcept;

	uint64_t getWidth() const noexcept;
	uint64_t getHeight() const noexcept;

	~Window() noexcept;
private:
	GLFWwindow* mpHandle;

	uint64_t mWidth;
	uint64_t mHeight;

	bool mDebugEnabled;
	bool mExitOverride;
	bool mCursorHidden;

	std::array<float, 4> mBackgroundColor;
	std::vector<std::pair<uint32_t, WindowActionCallback>> mKeyCallbacks;
	std::vector<WindowActionCallback> mGenericKeyCallbacks;
	std::vector<WindowActionCallback> mClickCallbacks;
	std::vector<WindowMouseCallback> mMouseCallbacks;

	Camera* mReferencedCamera;

	static void MouseCallback(GLFWwindow* aWindow, double aX, double aY) noexcept;
	static void KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aModifiers) noexcept;
	static void ClickCallback(GLFWwindow* aWindow, int aKey, int aAction, int aModifiers) noexcept;
	static void GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) noexcept;
	static void ResizeCallback(GLFWwindow* aWindow, int aNewX, int aNewY) noexcept;

	//defined in Camera.cpp
	void updateCamera() noexcept;
};

#endif
