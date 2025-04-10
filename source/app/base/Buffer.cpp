#include "Buffer.hpp"

VertexArray::VertexArray() noexcept
	: mHandle(0), mAttributeCounter(0) {
	glGenVertexArrays(1, &this->mHandle);
	glBindVertexArray(this->mHandle);
}
VertexArray::VertexArray(const bool aMakeDead) noexcept
	: mHandle(UINT32_MAX), mAttributeCounter(0) { DISCARD(aMakeDead); }
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

Vertex::Vertex() noexcept {
	for(uint64_t i = 0; i < MAX_BONES_PER_VERTEX; i++) {
		this->boneIds[i] = -1.0;
		this->boneWeights[i] = 0.0;
	}
}

Vertex::Vertex(const glm::vec3& aPosition) noexcept : position(aPosition) {
	for(uint64_t i = 0; i < MAX_BONES_PER_VERTEX; i++) {
		this->boneIds[i] = -1.0;
		this->boneWeights[i] = 0.0;
	}
}

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept {
	aStream << '{' << aVertex.position << ';' << aVertex.normal << ';' << aVertex.texCoords << '}';
	return aStream;
}

VertexBuffer::VertexBuffer(GLfloat* const arData, const uint64_t aVertices, const uint64_t aVerticesSize) noexcept
	: mVertices(aVertices), mVerticesSize(aVerticesSize), mHandledVertices(0) {
	if(this->mVertices * this->mVerticesSize == 0) {
		this->mHandle = 0;
		return;
	}
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
	glDeleteBuffers(1, &this->mHandle);

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
void VertexBuffer::enableStandardAttributes(VertexArray* const apVAO) noexcept {
	if(this->mVertices == 0) return; //ignore if empty buffer

	apVAO->bind();
	this->bind();

	//pos
	this->enableAttribute(apVAO, 3, 0, 0);
	//normal vector
	this->enableAttribute(apVAO, 3, 1, 3);
	//tex coords
	this->enableAttribute(apVAO, 2, 2, 6);
	//texture id
	this->enableAttribute(apVAO, 1, 3, 8);
	//bone ids
	this->enableAttribute(apVAO, MAX_BONES_PER_VERTEX, 4, 9);
	//bone weights
	this->enableAttribute(apVAO, MAX_BONES_PER_VERTEX, 5, 13);
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
	if(aSize == 0) {
		this->mHandle = 0;
		return;
	}
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
	glDeleteBuffers(1, &this->mHandle);

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
void IndexBuffer::drawInstanced(const uint64_t aCount) noexcept {
	glDrawElementsInstanced(GL_TRIANGLES, this->mSize, GL_UNSIGNED_INT, nullptr, aCount);
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
Framebuffer::Framebuffer(Framebuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	aOther.mHandle = 0;
}
Framebuffer& Framebuffer::operator=(Framebuffer&& aOther) noexcept {
	glDeleteFramebuffers(1, &this->mHandle);

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

bool Framebuffer::checkStatus() noexcept {
	glBindFramebuffer(GL_FRAMEBUFFER, this->mHandle);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << LogLevel::ERROR << " Framebuffer at address " << this << " is incomplete!\n" << LogLevel::RESET;
	}
	return true;
}

GLuint Framebuffer::getHandle() const noexcept {
	return this->mHandle;
}

//bind texture defined in texture.cpp

Framebuffer::~Framebuffer() noexcept {
	glDeleteFramebuffers(1, &this->mHandle);
}

ShaderBuffer::ShaderBuffer(const void* const arData, const uint64_t aSizeBytes) noexcept
: mHandle(0), mSizeBytes(aSizeBytes) {
	glGenBuffers(1, &this->mHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->mHandle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, aSizeBytes, arData, GL_DYNAMIC_DRAW);
}

ShaderBuffer::ShaderBuffer(ShaderBuffer&& aOther) noexcept
: mHandle(aOther.mHandle), mSizeBytes(aOther.mSizeBytes) {
	aOther.mHandle = 0;
}
ShaderBuffer& ShaderBuffer::operator=(ShaderBuffer&& aOther) noexcept {
	this->mHandle = aOther.mHandle;
	this->mSizeBytes = aOther.mSizeBytes;

	aOther.mHandle = 0;

	return *this;
}

void ShaderBuffer::setNewData(const void* const arData, const uint64_t aSizeBytes) noexcept {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->mHandle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, aSizeBytes, arData, GL_DYNAMIC_DRAW); //reallocate
	this->mSizeBytes = aSizeBytes;
}
void ShaderBuffer::update(const void* const arData, const uint64_t aSizeBytes, const uint64_t aOffset) noexcept {
	if(aOffset+aSizeBytes > this->mSizeBytes) {
		std::cerr << LogLevel::ERROR << "SSBO read out of range (" << aOffset+aSizeBytes << '>' << this->mSizeBytes << ")!\n" << LogLevel::RESET;
		std::exit(EXIT_FAILURE);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->mHandle);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, aOffset, aSizeBytes-aOffset, arData);
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
