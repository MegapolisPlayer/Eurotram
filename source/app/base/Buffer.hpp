#ifndef EUROTRAM_BUFFER
#define EUROTRAM_BUFFER
#include "FreeUtils.hpp"

class VertexArray {
	friend class VertexBuffer;
public:
	VertexArray() noexcept;
	VertexArray(VertexArray&& aOther) noexcept;
	VertexArray& operator=(VertexArray&& aOther) noexcept;
	VertexArray(VertexArray& aOther) noexcept = delete;
	VertexArray& operator=(VertexArray& aOther) noexcept = delete;

	void bind() noexcept;
	void unbind() noexcept;

	GLuint getHandle() noexcept;

	~VertexArray() noexcept;
private:
	GLuint mHandle;
	uint64_t mAttributeCounter;
};

class VertexBuffer {
public:
	VertexBuffer(GLfloat* const arData, const uint64_t aVertices, const uint64_t aVerticesSize) noexcept;
	VertexBuffer(VertexBuffer&& aOther) noexcept;
	VertexBuffer& operator=(VertexBuffer&& aOther) noexcept;
	VertexBuffer(VertexBuffer& aOther) noexcept = delete;
	VertexBuffer& operator=(VertexBuffer& aOther) noexcept = delete;

	void enableAttribute(VertexArray* const apVAO, const uint64_t aAmount) noexcept;
	void enableAttribute(VertexArray* const apVAO, const uint64_t aAmount, const uint64_t aOverrideCounter, const uint64_t aHandledOverride) noexcept;

	void bind() noexcept;
	void unbind() noexcept;

	//use as debug
	void drawPoints() noexcept;

	uint64_t getVerticesAmount() const noexcept;
	uint64_t getVertexSize() const noexcept;

	~VertexBuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mVertices;
	uint64_t mVerticesSize;
	uint64_t mHandledVertices;
};

class IndexBuffer {
public:
	IndexBuffer(GLuint* const arData, const uint64_t aSize) noexcept;
	IndexBuffer(IndexBuffer&& aOther) noexcept;
	IndexBuffer& operator=(IndexBuffer&& aOther) noexcept;
	IndexBuffer(IndexBuffer& aOther) noexcept = delete;
	IndexBuffer& operator=(IndexBuffer& aOther) noexcept = delete;

	void bind() noexcept;
	void unbind() noexcept;

	void draw() noexcept;

	uint64_t getHandle() const noexcept;
	uint64_t getSize() const noexcept;

	~IndexBuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mSize;
};

//used for implementing mirrors
class Framebuffer {
public:
	Framebuffer() noexcept;
	Framebuffer(Framebuffer&& aOther) noexcept;
	Framebuffer& operator=(Framebuffer&& aOther) noexcept;
	Framebuffer(Framebuffer& aOther) noexcept = delete;
	Framebuffer& operator=(Framebuffer& aOther) noexcept = delete;

	void bind() noexcept;
	void unbind() noexcept;

	uint64_t getHandle() const noexcept;
	uint64_t getWidth() const noexcept;
	uint64_t getHeight() const noexcept;

	~Framebuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mWidth, mHeight;
};

#endif
