#include "Window.hpp"

Window::Window(const char* aTitle, uint64_t aWidth, const uint64_t aHeight, const bool aFullscreen, const bool aDebug) noexcept
	: mpHandle(nullptr), mReferencedCamera(NULL), mDebugEnabled(aDebug), mExitOverride(false), mCursorHidden(false), mWidth(aWidth), mHeight(aHeight),
	 mBackgroundColor({1.0f, 0.0f, 1.0f, 1.0f}) {
	if(glfwInit() != GLFW_TRUE) {
		std::cerr << LogLevel::ERROR << "GLFW failed to initialize." << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, this->mDebugEnabled);

	glfwWindowHint(GLFW_SAMPLES, 4);

	glfwSetErrorCallback([](int aCode, const char* aString) {
		std::cerr << LogLevel::ERROR << "GLFW error " << aCode << ": " << aString << "\n" << LogLevel::RESET;
	});

	GLFWmonitor* monitor = aFullscreen ? glfwGetPrimaryMonitor() : NULL;
	this->mpHandle = glfwCreateWindow(aWidth, aHeight, aTitle, monitor, NULL);

	glfwSetInputMode(this->mpHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetKeyCallback(this->mpHandle, Window::KeyCallback);
	glfwSetMouseButtonCallback(this->mpHandle, Window::ClickCallback);
	glfwSetCursorPosCallback(this->mpHandle, Window::MouseCallback);
	glfwSetWindowSizeCallback(this->mpHandle, Window::ResizeCallback);
	glfwSetWindowUserPointer(this->mpHandle, this);

	glfwMakeContextCurrent(this->mpHandle);
	glfwSwapInterval(0); //disable VSYNC

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << LogLevel::ERROR << "GLAD failed to initialize. \n" << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	//we must support bindless textures

	glViewport(0, 0, this->mWidth, this->mHeight);

	if(this->mDebugEnabled) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	glPointSize(10); //set size of points, default 10

	glEnable(GL_BLEND); //texture blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST); //depth testing
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE); //backface culling
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_MULTISAMPLE); //anti aliasing

	glEnable(GL_FRAMEBUFFER_SRGB); //gamma correction

	glDebugMessageCallback(Window::GLCallback, nullptr);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(this->mpHandle, true);
	ImGui_ImplOpenGL3_Init("#version 450 core");
}

void Window::enableVSync() noexcept {
	glfwSwapInterval(1);
}

void Window::disableVSync() noexcept {
	glfwSwapInterval(0);
}

void Window::setBackgroundColor(const std::array<float, 4>& aArray) noexcept {
	this->mBackgroundColor = aArray;
}
void Window::setBackgroundColor(const glm::vec4& aArray) noexcept {
	this->mBackgroundColor[0] = aArray.r;
	this->mBackgroundColor[1] = aArray.g;
	this->mBackgroundColor[2] = aArray.b;
	this->mBackgroundColor[3] = aArray.a;
}

void Window::applyBackgroundColor() noexcept {
	glClearColor(this->mBackgroundColor[0], this->mBackgroundColor[1], this->mBackgroundColor[2], this->mBackgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::setViewport(const uint32_t aWidth, const uint32_t aHeight) noexcept {
	glViewport(0, 0, aWidth, aHeight);
	this->updateCamera();
}
void Window::resetViewport() noexcept {
	glViewport(0, 0, this->mWidth, this->mHeight);
	this->updateCamera();
}

void Window::bindContext() noexcept {
	glfwMakeContextCurrent(this->mpHandle);
}

bool Window::isOpen() const noexcept {
	return !(this->mExitOverride || glfwWindowShouldClose(this->mpHandle));
}

void Window::beginFrame() noexcept {
	this->applyBackgroundColor();

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

uint64_t Window::getWidth() const noexcept {
	return this->mWidth;
}
uint64_t Window::getHeight() const noexcept {
	return this->mHeight;
}

Window::~Window() noexcept {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(this->mpHandle);
}

void Window::MouseCallback(GLFWwindow* aWindow, double aX, double aY) noexcept {
	if(!glfwGetWindowAttrib(aWindow, GLFW_FOCUSED)) return;

	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowMouseCallback Callback : ClassPointer->mMouseCallbacks) {
		Callback(ClassPointer, aX, aY);
	}
}
void Window::KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int aModifiers) noexcept {
	if(!glfwGetWindowAttrib(aWindow, GLFW_FOCUSED)) return;

	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	for(WindowActionCallback Callback : ClassPointer->mGenericKeyCallbacks) {
		Callback(ClassPointer, aKey, aAction, aModifiers);
	}
	for(uint64_t Id = 0; Id < ClassPointer->mKeyCallbacks.size(); Id++) {
		if(ClassPointer->mKeyCallbacks[Id].first == (uint32_t)aKey) {
			ClassPointer->mKeyCallbacks[Id].second(ClassPointer, aKey, aAction, aModifiers);
		}
	}
}
void Window::ClickCallback(GLFWwindow* aWindow, int aKey, int aAction, int aModifiers) noexcept {
	if(!glfwGetWindowAttrib(aWindow, GLFW_FOCUSED)) return;

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
void Window::ResizeCallback(GLFWwindow* aWindow, int aNewX, int aNewY) noexcept {
	Window* ClassPointer = (Window*)glfwGetWindowUserPointer(aWindow);
	ClassPointer->mWidth = aNewX;
	ClassPointer->mHeight = aNewY;
	glViewport(0, 0, ClassPointer->mWidth, ClassPointer->mHeight);
}
