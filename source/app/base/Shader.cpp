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
