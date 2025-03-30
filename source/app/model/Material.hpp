#ifndef EUROTRAM_MATERIAL
#define EUROTRAM_MATERIAL
#include "../base/Base.hpp"

constexpr float MAX_BRIGHTNESS = 255.0;

struct alignas(16) Material {
	glm::vec4 color = glm::vec4(1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	GLfloat ior = 1.0f;
	GLfloat shininess = 1.0f;

	GLfloat textureAmount = 1.0f; //1.0 texture only, 0.0 color only
	GLint textureSlot = 0;
	GLfloat textureOpacity = 1.0f;
	GLfloat brightness = 0.0f; //below this brightness render as normal, above it is brighter (range to 255!!!!)
};

#define GMS_STANDARD_IDENTIFICATOR "GMSDONOTUSETHISID"

struct GMSVariant {
	uint64_t baseId;
	std::vector<uint64_t> variants;
	int64_t currentVariant; //-1 is standard
};

//standard always before derived
struct GMSEntry {
	Material material;
	Texture texture;
	std::string name;
	std::string path;
	std::string variant = GMS_STANDARD_IDENTIFICATOR;

	GMSVariant* variantData = nullptr;

	GMSEntry* duplicateOf = nullptr; //set for duplicates

	GMSEntry() noexcept;
};

std::ostream& operator<<(std::ostream& aStream, const GMSEntry& aEntry) noexcept;

class UniformMaterial : public StructUniform<Material> {
public:
	UniformMaterial(const uint64_t aLocation, const uint64_t aAmount = 1) noexcept;
	virtual ~UniformMaterial() noexcept;
private:
};

//stores and handles all materials
class GlobalMaterialStore {
public:
	GlobalMaterialStore(GlobalMaterialStore&) noexcept = delete;
	GlobalMaterialStore(GlobalMaterialStore&&) noexcept = delete;
	GlobalMaterialStore& operator=(GlobalMaterialStore&) noexcept = delete;
	GlobalMaterialStore& operator=(GlobalMaterialStore&&) noexcept = delete;

	//returns new element's id via pointer
	static GMSEntry* add(uint64_t* aNewElementId = nullptr) noexcept;
	static GMSEntry* copyStandard(const std::string_view aMaterialName) noexcept;

	//end > start! also binds textures to slots
	static void copyDataToUniform(UniformMaterial& aUniform, uint64_t aStart = 0, uint64_t aEnd = GlobalMaterialStore::msContainer.size()) noexcept;

	static GMSEntry* getByName(const std::string_view aMaterialName) noexcept;
	static GMSEntry* getByPath(const std::string_view aPath) noexcept;

	//returns new element's id via pointer
	static GMSEntry* get(const std::string_view aMaterialName, const std::string_view aVariant = GMS_STANDARD_IDENTIFICATOR) noexcept;
	static GMSEntry* getStandard(const std::string_view aMaterialName) noexcept;

	static GMSEntry* getById(const uint64_t aId) noexcept;
	static uint64_t findId(const std::string_view aMaterialName, const std::string_view aVariant = GMS_STANDARD_IDENTIFICATOR) noexcept;

	static uint64_t getLength() noexcept;

	//adds new material and registers it as variant
	static GMSEntry* addVariant(const std::string_view aMaterialName, const std::string_view aNewVariantName) noexcept;

	//converts separate material aMaterialVariantName to variant of aMaterialName
	static GMSEntry* addVariantFromMaterial(const std::string_view aMaterialName, const std::string_view aIdentifier, const std::string_view aMaterialVariantName) noexcept;

	static void setVariant(const std::string_view aMaterialName, const std::string_view aIdentifier) noexcept;
	static void resetVariant(const std::string_view aMaterialName) noexcept;

	static void setVariantAll(const std::string_view aIdentifier) noexcept;
	static void resetVariantAll() noexcept;

	static GMSVariant* getVariantById(const uint64_t aId) noexcept;

	//updates all entries with name, variants set to same color as base
	static void randomizeColors(const std::string_view aMaterialName) noexcept;

	static std::ostream& printData(std::ostream& aStream = std::cout) noexcept;

	static std::list<GMSEntry>& getEntryList() noexcept;

	~GlobalMaterialStore() noexcept;
private:
	GlobalMaterialStore() noexcept;

	static std::list<GMSEntry> msContainer;
	static std::list<GMSVariant> msVariantBindings;
};

#endif
