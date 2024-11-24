#include "Window.hpp"

Window::Window(const char* aTitle, uint64_t aWidth, const uint64_t aHeight, const bool aFullscreen, const bool aDebug) noexcept {
	if(glfwInit() != GLFW_TRUE) {
		std::cerr << "GLFW failed to initialize. (Window)";
		std::exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	GLFWmonitor* Monitor = aFullscreen ? glfwGetPrimaryMonitor() : NULL;
	this->mpHandle = glfwCreateWindow(aWidth, aHeight, aTitle, Monitor, NULL);

	glfwSetInputMode(this->mpHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetKeyCallback(this->mpHandle, Window::KeyCallback);
	glfwSetCursorPosCallback(this->mpHandle, Window::MouseCallback);
	glfwSetWindowUserPointer(this->mpHandle, this);

	glfwMakeContextCurrent(this->mpHandle);

	if(glewInit() != GLEW_OK) {
		std::cerr << "GLEW failed to initialize. (Window)";
		std::exit(EXIT_FAILURE);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glDebugMessageCallback(Window::GLCallback, nullptr);
}

void Window::bindContext() noexcept {
	glDisable(GL_DEBUG_OUTPUT);
	glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
	glfwMakeContextCurrent(this->mpHandle);
}
void Window::bindDebugContext() noexcept {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwMakeContextCurrent(this->mpHandle);
}

bool Window::isOpen() const noexcept {
	return !(this->mExitOverride || glfwWindowShouldClose(this->mpHandle));
}

void Window::clear() noexcept {
	glClearColor(this->mBackgroundColor[0], this->mBackgroundColor[1], this->mBackgroundColor[2], this->mBackgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Window::update() noexcept {
	glfwSwapBuffers(this->mpHandle);
	glfwPollEvents();
}

void Window::hideCursor() noexcept {
	glfwSetInputMode(this->mpHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void Window::showCursor() noexcept {
	glfwSetInputMode(this->mpHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::registerKeyCallback(const uint32_t aGLFWKey, WindowKeyCallback aCallback) noexcept {
	this->mKeyCallbacks.push_back(std::make_pair(aGLFWKey, aCallback));
}
uint32_t Window::registerGenericKeyCallback(WindowKeyCallback aCallback) noexcept {
	this->mGenericKeyCallbacks.push_back(aCallback);
	return this->mMouseCallbacks.size() - 1;
}
uint32_t Window::registerMouseCallback(WindowMouseCallback aCallback) noexcept {
	this->mMouseCallbacks.push_back(aCallback);
	return this->mMouseCallbacks.size() - 1;
}
void Window::removeKeyCallback(const uint32_t aGLFWKey) noexcept {
	for(uint64_t Id = this->mKeyCallbacks.size() - 1; Id >= 0; Id--) {
		if(this->mKeyCallbacks[Id].first == aGLFWKey) {
			this->mKeyCallbacks.erase(this->mKeyCallbacks.begin() + Id);
		}
	}
}
void Window::removeGenericKeyCallback(const uint32_t aId) noexcept {
	this->mGenericKeyCallbacks.erase(this->mGenericKeyCallbacks.begin() + aId);
}
void Window::removeMouseCallback(const uint32_t aId) noexcept {
	this->mMouseCallbacks.erase(this->mMouseCallbacks.begin() + aId);
}

void Window::close() noexcept {
	this->mExitOverride = true;
}

Window::~Window() noexcept {
	glfwDestroyWindow(this->mpHandle);
}

void Window::MouseCallback(GLFWwindow* aWindow, double aX, double aY) {
	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowMouseCallback Callback : ClassPointer->mMouseCallbacks) {
		Callback(ClassPointer, aX, aY);
	}
}
void Window::KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aModifiers) {
	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowKeyCallback Callback : ClassPointer->mGenericKeyCallbacks) {
		Callback(ClassPointer, aKey, aAction, aModifiers);
	}
	for(uint64_t Id = 0; Id < ClassPointer->mKeyCallbacks.size(); Id++) {
		if(ClassPointer->mKeyCallbacks[Id].first == aKey) {
			ClassPointer->mKeyCallbacks[Id].second(ClassPointer, aKey, aAction, aModifiers);
		}
	}
}
void Window::GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cerr << "OpenGL (";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cerr << "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cerr << "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cerr << "Info"; break;
		default: break;
	}
	std::cerr << "): ";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cerr << "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cerr << "Deprecation"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cerr << "Undefined behaviour/bug"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cerr << "Performance"; break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cerr << "Portability"; break;
		default:
			std::cerr << "Unknown (" << type << ")";
	}
	std::cerr << ": " << message << " (" << id << ")" << std::endl;
}
