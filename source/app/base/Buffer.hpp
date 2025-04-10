#ifndef EUROTRAM_BUFFER
#define EUROTRAM_BUFFER
#include "FreeUtils.hpp"

class VertexArray {
	friend class VertexBuffer;
public:
	VertexArray() noexcept;
	//if aMakeDead is set to true - creates empty array object, without value, which may only be overriden by another
	VertexArray(const bool aMakeDead) noexcept;
	VertexArray(VertexArray&& aOther) noexcept;
	VertexArray& operator=(VertexArray&& aOther) noexcept;
	VertexArray(VertexArray& aOther) noexcept = delete;
	VertexArray& operator=(VertexArray& aOther) noexcept = delete;

	void bind() noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	~VertexArray() noexcept;
private:
	GLuint mHandle;
	uint64_t mAttributeCounter;
};

constexpr uint64_t MAX_BONES_PER_VERTEX = 4;
constexpr uint64_t STANDARD_MODEL_VERTEX_FLOAT_AMOUNT = 9+MAX_BONES_PER_VERTEX+MAX_BONES_PER_VERTEX;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	GLfloat materialId;

	GLfloat boneIds[MAX_BONES_PER_VERTEX]; //we save as float but IDs are integers
	GLfloat boneWeights[MAX_BONES_PER_VERTEX];

	Vertex() noexcept;
	Vertex(const glm::vec3& aPosition) noexcept;
};

std::ostream& operator<<(std::ostream& aStream, const Vertex& aVertex) noexcept;

class VertexBuffer {
public:
	VertexBuffer(GLfloat* const arData, const uint64_t aVertices, const uint64_t aVerticesSize) noexcept;
	VertexBuffer(VertexBuffer&& aOther) noexcept;
	VertexBuffer& operator=(VertexBuffer&& aOther) noexcept;
	VertexBuffer(VertexBuffer& aOther) noexcept = delete;
	VertexBuffer& operator=(VertexBuffer& aOther) noexcept = delete;

	void enableAttribute(VertexArray* const apVAO, const uint64_t aAmount) noexcept;
	void enableAttribute(VertexArray* const apVAO, const uint64_t aAmount, const uint64_t aOverrideCounter, const uint64_t aHandledOverride) noexcept;

	void enableStandardAttributes(VertexArray* const apVAO) noexcept;

	void bind() noexcept;
	void unbind() noexcept;

	//use as debug
	void drawPoints() noexcept;

	GLuint getHandle() const noexcept;
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
	void drawInstanced(const uint64_t aCount) noexcept;

	GLuint getHandle() const noexcept;
	uint64_t getSize() const noexcept;

	~IndexBuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mSize;
};

class Texture;
class FramebufferMultisampleTexture;

//used for implementing mirrors and shadows
class Framebuffer {
public:
	Framebuffer() noexcept;
	Framebuffer(Framebuffer&& aOther) noexcept;
	Framebuffer& operator=(Framebuffer&& aOther) noexcept;
	Framebuffer(Framebuffer& aOther) noexcept = delete;
	Framebuffer& operator=(Framebuffer& aOther) noexcept = delete;

	void bind() noexcept;
	void unbind() noexcept;

	void bindAsRead() noexcept;
	void unbindAsRead() noexcept;

	void bindTexture(const Texture& aTexture, const uint64_t aFormat, const bool aRead = false, const bool aDraw = false) noexcept;
	void bindTexture(const FramebufferMultisampleTexture& aTexture, const uint64_t aFormat, const bool aRead = false, const bool aDraw = false) noexcept;

	bool checkStatus() noexcept;

	GLuint getHandle() const noexcept;

	~Framebuffer() noexcept;
private:
	GLuint mHandle;
};

class ShaderBuffer {
public:
	ShaderBuffer(const void* const arData, const uint64_t aSizeBytes) noexcept;
	ShaderBuffer(ShaderBuffer&& aOther) noexcept;
	ShaderBuffer& operator=(ShaderBuffer&& aOther) noexcept;
	ShaderBuffer(ShaderBuffer& aOther) noexcept = delete;
	ShaderBuffer& operator=(ShaderBuffer& aOther) noexcept = delete;

	void setNewData(const void* const arData, const uint64_t aSizeBytes) noexcept; //sets new data
	void update(const void* const arData, const uint64_t aSizeBytes, const uint64_t aOffset = 0) noexcept;

	void bind(const uint64_t aBindLocation) noexcept;
	void unbind() noexcept;

	GLuint getHandle() const noexcept;

	~ShaderBuffer() noexcept;
private:
	GLuint mHandle;
	uint64_t mSizeBytes;
};

#endif
