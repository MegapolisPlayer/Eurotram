#include "Buffer.hpp"

VertexArray::VertexArray() noexcept
	: mHandle(0), mAttributeCounter(0) {
	glGenVertexArrays(1, &this->mHandle);
	glBindVertexArray(this->mHandle);
}
void VertexArray::bind() noexcept {
	glBindVertexArray(this->mHandle);
}
void VertexArray::unbind() noexcept {
	glBindVertexArray(0);
}

GLuint VertexArray::getHandle() noexcept {
	return this->mHandle;
}
VertexArray::~VertexArray() noexcept {
	glDeleteVertexArrays(1, &this->mHandle);
}

VertexBuffer::VertexBuffer(GLfloat* const arData, const uint64_t aVertices, const uint64_t aVerticesSize) noexcept
	: mVertices(aVertices), mVerticesSize(aVerticesSize), mHandledVertices(0) {
	glGenBuffers(1, &this->mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, this->mHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->mVertices * this->mVerticesSize, arData, GL_STATIC_DRAW);
}
void VertexBuffer::enableAttribute(VertexArray* const apVAO, const uint64_t aAmount) noexcept {
	apVAO->bind();
	glVertexAttribPointer(apVAO->mAttributeCounter, aAmount, GL_FLOAT, GL_FALSE, this->mVerticesSize * sizeof(GLfloat), (const void*)(this->mHandledVertices*sizeof(GLfloat)));
	glEnableVertexAttribArray(apVAO->mAttributeCounter);
	apVAO->mAttributeCounter++;
	this->mHandledVertices += aAmount;
}
void VertexBuffer::bind() noexcept {
	glBindBuffer(GL_ARRAY_BUFFER, this->mHandle);
}
void VertexBuffer::unbind() noexcept {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
uint64_t VertexBuffer::getVerticesAmount() const noexcept {
	return this->mVertices;
}
uint64_t VertexBuffer::getVertexSize() const noexcept {
	return this->mVerticesSize;
}
VertexBuffer::~VertexBuffer() noexcept {
	glDeleteBuffers(1, &this->mHandle);
}

IndexBuffer::IndexBuffer(GLuint* const arData, const uint64_t aSize) noexcept
	: mHandle(0), mSize(aSize) {
	glGenBuffers(1, &this->mHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->mSize, arData, GL_STATIC_DRAW);
}
void IndexBuffer::bind() noexcept {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mSize);
}
void IndexBuffer::unbind() noexcept {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void IndexBuffer::draw() noexcept {
	glDrawElements(GL_TRIANGLES, this->mSize, GL_UNSIGNED_INT, nullptr);
}
uint64_t IndexBuffer::getSize() const noexcept {
	return this->mSize;
}
IndexBuffer::~IndexBuffer() noexcept {
	glDeleteBuffers(1, &this->mHandle);
}
