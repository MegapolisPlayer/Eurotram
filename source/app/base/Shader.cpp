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
		std::cout << LogLevel::ERROR << "Vertex shader compilation failed: " << message << '\n' << LogLevel::RESET;
	}
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		failed = true;
		glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &success);
		std::string message;
		message.resize(success);
		glGetShaderInfoLog(fragment, GL_INFO_LOG_LENGTH, NULL, &message[0]);
		std::cout << LogLevel::ERROR << "Fragment shader compilation failed: " << message << '\n' << LogLevel::RESET;
	}

	if(failed) { std::exit(EXIT_FAILURE); }

	glDetachShader(this->mHandle, vertex);
	glDetachShader(this->mHandle, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(this->mHandle);
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

ShaderBuffer::ShaderBuffer(void* const arData, const uint64_t aSizeBytes) noexcept {
	glGenBuffers(1, &this->mHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->mHandle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, aSizeBytes, arData, GL_STATIC_DRAW);
}
void ShaderBuffer::update(void* const arData, const uint64_t aSizeBytes) noexcept {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->mHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, aSizeBytes, arData);
}
void ShaderBuffer::bind(const uint64_t aBindLocation) noexcept {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, aBindLocation, this->mHandle);
}
void ShaderBuffer::unbind() noexcept {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
GLuint ShaderBuffer::getHandle() const noexcept {
	return this->mHandle;
}
ShaderBuffer::~ShaderBuffer() noexcept {
	glDeleteBuffers(1, &this->mHandle);
}

