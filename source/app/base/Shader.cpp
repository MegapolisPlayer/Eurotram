#include "Shader.hpp"

Shader::Shader(const std::string_view aVertexSource, const std::string_view aFragmentSource) noexcept {
	std::fstream fileLoader;

	this->mHandle = glCreateProgram();
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

	{
		std::string source = readFile(fileLoader, aVertexSource);
		const char* sourcePtr =	source.c_str();
		GLint sourceSize = source.size();
		glShaderSource(vertex, 1, &sourcePtr, &sourceSize);
		glCompileShader(vertex);
	}
	{
		std::string source = readFile(fileLoader, aFragmentSource);
		const char* sourcePtr =	source.c_str();
		GLint sourceSize = source.size();
		glShaderSource(fragment, 1, &sourcePtr, &sourceSize);
		glCompileShader(fragment);
	}

	glAttachShader(this->mHandle, vertex);
	glAttachShader(this->mHandle, fragment);

	glLinkProgram(this->mHandle);
	glValidateProgram(this->mHandle);

	bool failed = false;

	GLint success = 0;
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		failed = true;
		glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &success);
		std::string message;
		message.resize(success);
		glGetShaderInfoLog(vertex, GL_INFO_LOG_LENGTH, NULL, &message[0]);
		std::cout << LogLevel::ERROR << aVertexSource << " - Vertex shader compilation failed: " << message << '\n' << LogLevel::RESET;
	}
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		failed = true;
		glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &success);
		std::string message;
		message.resize(success);
		glGetShaderInfoLog(fragment, GL_INFO_LOG_LENGTH, NULL, &message[0]);
		std::cout << LogLevel::ERROR << aFragmentSource << " - Fragment shader compilation failed: " << message << '\n' << LogLevel::RESET;
	}
	glGetProgramiv(this->mHandle, GL_LINK_STATUS, &success);
	if(success == GL_FALSE) {
		failed = true;
		glGetProgramiv(this->mHandle, GL_INFO_LOG_LENGTH, &success);
		std::string message;
		message.resize(success);
		glGetProgramInfoLog(this->mHandle, GL_INFO_LOG_LENGTH, NULL, &message[0]);
		std::cout << LogLevel::ERROR << "Shader link failed: " << message << '\n' << LogLevel::RESET;
	}

	//first print all messages, then exit
	if(failed) { std::exit(EXIT_FAILURE); }

	glDetachShader(this->mHandle, vertex);
	glDetachShader(this->mHandle, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(this->mHandle);
}

Shader::Shader(Shader&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	aOther.mHandle = 0;
}
Shader& Shader::operator=(Shader&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	aOther.mHandle = 0;
	return *this;
}

void Shader::bind() noexcept {
	glUseProgram(this->mHandle);
}
void Shader::unbind() noexcept {
	glUseProgram(0);
}
GLuint Shader::getHandle() const noexcept {
	return this->mHandle;
}
Shader::~Shader() {
	glDeleteProgram(this->mHandle);
}
