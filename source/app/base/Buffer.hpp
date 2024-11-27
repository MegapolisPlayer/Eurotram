#ifndef EUROTRAM_BUFFER
#define EUROTRAM_BUFFER
#include "Setup.hpp"

class VertexArray {
	friend class VertexBuffer;
public:
	VertexArray() noexcept;

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

	void enableAttribute(VertexArray* const apVAO, const uint64_t aAmount) noexcept;

	void bind() noexcept;
	void unbind() noexcept;

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

	void bind() noexcept;
	void unbind() noexcept;

	void draw() noexcept;

	uint64_t getSize() const noexcept;

	~IndexBuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mSize;
};

#endif
