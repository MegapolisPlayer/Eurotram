#include "Shader.hpp"

Shader::Shader(const std::string_view aVertexSource, const std::string_view aFragmentSource) noexcept {
	std::fstream fileLoader;

	this->mHandle = glCreateProgram();
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

	{
		std::string Source = readFile(fileLoader, aVertexSource);
		const char* SourcePtr =	Source.c_str();
		glShaderSource(vertex, 1, &SourcePtr, NULL);
		glCompileShader(vertex);
	}
	{
		std::string Source = readFile(fileLoader, aFragmentSource);
		const char* SourcePtr =	Source.c_str();
		glShaderSource(fragment, 1, &SourcePtr, NULL);
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
