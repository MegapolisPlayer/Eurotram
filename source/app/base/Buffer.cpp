#include "Buffer.hpp"

VertexArray::VertexArray() noexcept
	: mHandle(0), mAttributeCounter(0) {
	glGenVertexArrays(1, &this->mHandle);
	glBindVertexArray(this->mHandle);
}
VertexArray::VertexArray(VertexArray&& aOther) noexcept
	: mHandle(aOther.mHandle), mAttributeCounter(aOther.mAttributeCounter) {
	aOther.mHandle = 0;
}
VertexArray& VertexArray::operator=(VertexArray&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	this->mAttributeCounter = aOther.mAttributeCounter;

	aOther.mHandle = 0;

	return *this;
}
void VertexArray::bind() noexcept {
	glBindVertexArray(this->mHandle);
}
void VertexArray::unbind() noexcept {
	glBindVertexArray(0);
}

GLuint VertexArray::getHandle() const noexcept {
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
VertexBuffer::VertexBuffer(VertexBuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	this->mVertices = aOther.mVertices;
	this->mVerticesSize = aOther.mVerticesSize;
	this->mHandledVertices = aOther.mHandledVertices;

	aOther.mHandle = 0;
}
VertexBuffer& VertexBuffer::operator=(VertexBuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	this->mVertices = aOther.mVertices;
	this->mVerticesSize = aOther.mVerticesSize;
	this->mHandledVertices = aOther.mHandledVertices;

	aOther.mHandle = 0;

	return *this;
}
void VertexBuffer::enableAttribute(VertexArray* const apVAO, const uint64_t aAmount) noexcept {
	apVAO->bind();
	glVertexAttribPointer(apVAO->mAttributeCounter, aAmount, GL_FLOAT, GL_FALSE, this->mVerticesSize * sizeof(GLfloat), (const void*)(this->mHandledVertices*sizeof(GLfloat)));
	glEnableVertexAttribArray(apVAO->mAttributeCounter);
	apVAO->mAttributeCounter++;
	this->mHandledVertices += aAmount;
}
void VertexBuffer::enableAttribute(VertexArray* const apVAO, const uint64_t aAmount, const uint64_t aOverrideCounter, const uint64_t aHandledOverride) noexcept {
	apVAO->bind();
	glVertexAttribPointer(aOverrideCounter, aAmount, GL_FLOAT, GL_FALSE, this->mVerticesSize * sizeof(GLfloat), (const void*)(aHandledOverride*sizeof(GLfloat)));
	glEnableVertexAttribArray(aOverrideCounter);
}
void VertexBuffer::bind() noexcept {
	glBindBuffer(GL_ARRAY_BUFFER, this->mHandle);
}
void VertexBuffer::unbind() noexcept {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VertexBuffer::drawPoints() noexcept {
	glDrawArrays(GL_POINTS, 0, this->mVertices);
}
GLuint VertexBuffer::getHandle() const noexcept {
	return this->mHandle;
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
	: mSize(aSize) {
	glGenBuffers(1, &this->mHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->mSize, arData, GL_STATIC_DRAW);
}
IndexBuffer::IndexBuffer(IndexBuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	this->mSize = aOther.mSize;

	aOther.mHandle = 0;
}
IndexBuffer& IndexBuffer::operator=(IndexBuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	this->mSize = aOther.mSize;

	aOther.mHandle = 0;

	return *this;
}
void IndexBuffer::bind() noexcept {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mHandle);
}
void IndexBuffer::unbind() noexcept {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void IndexBuffer::draw() noexcept {
	glDrawElements(GL_TRIANGLES, this->mSize, GL_UNSIGNED_INT, nullptr);
}
GLuint IndexBuffer::getHandle() const noexcept {
	return this->mHandle;
}
uint64_t IndexBuffer::getSize() const noexcept {
	return this->mSize;
}
IndexBuffer::~IndexBuffer() noexcept {
	glDeleteBuffers(1, &this->mHandle);
}

Framebuffer::Framebuffer() noexcept : mHandle(0) {
	glGenFramebuffers(1, &this->mHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, this->mHandle);
}
Framebuffer::Framebuffer(Framebuffer&& aOther) noexcept
	: mHandle(aOther.mHandle) {
	aOther.mHandle = 0;
}
Framebuffer& Framebuffer::operator=(Framebuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	aOther.mHandle = 0;

	return *this;
}

void Framebuffer::bind() noexcept {
	glBindFramebuffer(GL_FRAMEBUFFER, this->mHandle);
}
void Framebuffer::unbind() noexcept {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindAsRead() noexcept {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->mHandle);
}
void Framebuffer::unbindAsRead() noexcept {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

GLuint Framebuffer::getHandle() const noexcept {
	return this->mHandle;
}

//bind texture defined in texture.cpp

Framebuffer::~Framebuffer() noexcept {
	glDeleteFramebuffers(1, &this->mHandle);
}
