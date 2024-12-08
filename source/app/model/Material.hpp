#ifndef EUROTRAM_MATERIAL
#define EUROTRAM_MATERIAL
#include "../base/Base.hpp"

struct alignas(16) Material {
	glm::vec4 color = glm::vec4(1.0f);
	GLfloat diffuse = 1.0f;
	GLfloat specular = 1.0f;
	GLfloat shininess = 1.0f;

	GLfloat textureAmount = 1.0f; //1.0 texture only, 0.0 color only
	GLint textureSlot = 0;
	GLfloat brightness = 0.0f; //below this brightness render as normal, above it is brighter
};

class UniformMaterial : public StructUniform<Material> {
public:
	UniformMaterial(const uint64_t aLocation) noexcept;
	virtual ~UniformMaterial() noexcept;
private:
};

#endif
