#ifndef EUROTRAM_MATERIAL
#define EUROTRAM_MATERIAL
#include "../base/Base.hpp"

struct alignas(16) Material {
	glm::vec4 color = glm::vec4(1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	GLfloat ior = 1.0f;
	GLfloat shininess = 1.0f;

	GLfloat textureAmount = 1.0f; //1.0 texture only, 0.0 color only
	GLint textureSlot = 0;
	GLfloat textureOpacity = 1.0f;
	GLfloat brightness = 0.0f; //below this brightness render as normal, above it is brighter
};

#define GMS_STANDARD_IDENTIFICATOR "GMSDONOTUSETHISID"

struct GMSEntry {
	Material material;
	Texture texture;
	std::string name;
	std::string path;
	std::string variant = GMS_STANDARD_IDENTIFICATOR;

	GMSEntry() noexcept;
};

std::ostream& operator<<(std::ostream& aStream, const GMSEntry& aEntry) noexcept;

class GlobalMaterialStore {
public:
	GlobalMaterialStore(GlobalMaterialStore&) noexcept = delete;
	GlobalMaterialStore(GlobalMaterialStore&&) noexcept = delete;
	GlobalMaterialStore& operator=(GlobalMaterialStore&) noexcept = delete;
	GlobalMaterialStore& operator=(GlobalMaterialStore&&) noexcept = delete;

	static GMSEntry* add() noexcept;
	//responsibility to add texture and variant string lies with the user
	static GMSEntry* copyStandard(const std::string_view aMaterialName) noexcept;
	static GMSEntry* getByName(const std::string_view aMaterialName) noexcept;
	static GMSEntry* getByPath(const std::string_view aPath) noexcept;

	static GMSEntry* get(const std::string_view aMaterialName, const std::string_view aVariant) noexcept;
	static GMSEntry* getStandard(const std::string_view aMaterialName) noexcept;

	static GMSEntry* getById(const uint64_t aId) noexcept;

	static uint64_t getLength() noexcept;

	static std::ostream& printData(std::ostream& aStream = std::cout) noexcept;

	~GlobalMaterialStore() noexcept;
private:
	GlobalMaterialStore() noexcept;

	static std::list<GMSEntry> msContainer;
};


class UniformMaterial : public StructUniform<Material> {
public:
	UniformMaterial(const uint64_t aLocation, const uint64_t aAmount = 1) noexcept;
	virtual ~UniformMaterial() noexcept;
private:
};

#endif
