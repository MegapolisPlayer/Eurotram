#include "Window.hpp"

Window::Window(const char* aTitle, uint64_t aWidth, const uint64_t aHeight, const bool aFullscreen, const bool aDebug) noexcept
	: mBackgroundColor({1.0f, 0.0f, 1.0f, 1.0f}), mDebugEnabled(aDebug), mExitOverride(false), mCursorHidden(false),
	mWidth(aWidth), mHeight(aHeight), mReferencedCamera(NULL) {
	if(glfwInit() != GLFW_TRUE) {
		std::cerr << LogLevel::ERROR << "GLFW failed to initialize." << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, this->mDebugEnabled);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWmonitor* monitor = aFullscreen ? glfwGetPrimaryMonitor() : NULL;
	this->mpHandle = glfwCreateWindow(aWidth, aHeight, aTitle, monitor, NULL);

	glfwSetInputMode(this->mpHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetKeyCallback(this->mpHandle, Window::KeyCallback);
	glfwSetMouseButtonCallback(this->mpHandle, Window::ClickCallback);
	glfwSetCursorPosCallback(this->mpHandle, Window::MouseCallback);
	glfwSetWindowUserPointer(this->mpHandle, this);

	glfwSetErrorCallback([](int aCode, const char* aString) {
		std::cerr << LogLevel::ERROR << "GLFW error " << aCode << ": " << aString << "\n" << LogLevel::RESET;
	});

	glfwMakeContextCurrent(this->mpHandle);
	glfwSwapInterval(0); //disable VSYNC

	int result = glewInit();
	if(result != GLEW_OK) {
		std::cerr << LogLevel::ERROR << "GLEW failed to initialize. " << glewGetErrorString(result) << " (" << result << ")\n" << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	if(this->mDebugEnabled) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_MULTISAMPLE); //anti aliasing

	glDebugMessageCallback(Window::GLCallback, nullptr);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(this->mpHandle, true);
	ImGui_ImplOpenGL3_Init("#version 450 core");
}

void Window::setBackgroundColor(const std::array<float, 4>& aArray) noexcept {
	this->mBackgroundColor = aArray;
}

void Window::bindContext() noexcept {
	glfwMakeContextCurrent(this->mpHandle);
}

bool Window::isOpen() const noexcept {
	return !(this->mExitOverride || glfwWindowShouldClose(this->mpHandle));
}

void Window::beginFrame() noexcept {
	glClearColor(this->mBackgroundColor[0], this->mBackgroundColor[1], this->mBackgroundColor[2], this->mBackgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void Window::endFrame() noexcept {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwPollEvents();
	glfwSwapBuffers(this->mpHandle);

	this->updateCamera(); //defined in Camera.cpp
}

void Window::hideCursor() noexcept {
	glfwSetInputMode(this->mpHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	this->mCursorHidden = true;
}
void Window::showCursor() noexcept {
	glfwSetInputMode(this->mpHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	this->mCursorHidden = false;
}

void Window::registerKeyCallback(const uint32_t aGLFWKey, WindowActionCallback aCallback) noexcept {
	this->mKeyCallbacks.push_back(std::make_pair(aGLFWKey, aCallback));
}
uint32_t Window::registerClickCallback(WindowActionCallback aCallback) noexcept {
	this->mClickCallbacks.push_back(aCallback);
	return this->mClickCallbacks.size() - 1;
}
uint32_t Window::registerGenericKeyCallback(WindowActionCallback aCallback) noexcept {
	this->mGenericKeyCallbacks.push_back(aCallback);
	return this->mGenericKeyCallbacks.size() - 1;
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
void Window::removeClickCallback(const uint32_t aId) noexcept {
	this->mClickCallbacks.erase(this->mClickCallbacks.begin() + aId);
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

bool Window::isCursorHidden() const noexcept {
	return this->mCursorHidden;
}

void Window::bindCamera(Camera* const aCamera) noexcept {
	this->mReferencedCamera = aCamera;
}
Camera* Window::getCamera() const noexcept {
	return this->mReferencedCamera;
}

Window::~Window() noexcept {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(this->mpHandle);
}

void Window::MouseCallback(GLFWwindow* aWindow, double aX, double aY) noexcept {
	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowMouseCallback Callback : ClassPointer->mMouseCallbacks) {
		Callback(ClassPointer, aX, aY);
	}
}
void Window::KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aModifiers) noexcept {
	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowActionCallback Callback : ClassPointer->mGenericKeyCallbacks) {
		Callback(ClassPointer, aKey, aAction, aModifiers);
	}
	for(uint64_t Id = 0; Id < ClassPointer->mKeyCallbacks.size(); Id++) {
		if(ClassPointer->mKeyCallbacks[Id].first == aKey) {
			ClassPointer->mKeyCallbacks[Id].second(ClassPointer, aKey, aAction, aModifiers);
		}
	}
}
void Window::ClickCallback(GLFWwindow* aWindow, int aKey, int aAction, int aModifiers) noexcept {
	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowActionCallback Callback : ClassPointer->mClickCallbacks) {
		Callback(ClassPointer, aKey, aAction, aModifiers);
	}
}
void Window::GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) noexcept {
	std::cerr << "OpenGL (";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << LogLevel::ERROR << "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cerr << LogLevel::WARNING << "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cerr << LogLevel::WARNING << "Low"; break;
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
	std::cerr << ": " << message << " (" << id << ")" << LogLevel::RESET << std::endl;
}
