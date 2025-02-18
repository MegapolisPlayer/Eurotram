#include "Material.hpp"

GMSEntry::GMSEntry() noexcept : texture("") {}

std::ostream& operator<<(std::ostream& aStream, const GMSEntry& aEntry) noexcept {
	aStream << '[' << aEntry.name << '@' << aEntry.path << ':' << aEntry.variant << ']';
	return aStream;
}

std::list<GMSEntry> GlobalMaterialStore::msContainer;
std::list<GMSVariant> GlobalMaterialStore::msVariantBindings;

GMSEntry* GlobalMaterialStore::add(uint64_t* aNewElementId) noexcept {
	msContainer.emplace_back();
	if(aNewElementId) {
		*aNewElementId = msContainer.size()-1;
	}
	return &msContainer.back();
}

GMSEntry* GlobalMaterialStore::copyStandard(const std::string_view aMaterialName) noexcept {
	GMSEntry* source = getStandard(aMaterialName);
	msContainer.emplace_back();
	msContainer.back().material = source->material;
	msContainer.back().name = source->name;
	msContainer.back().variant = "";
	return &msContainer.back();
}

void GlobalMaterialStore::copyDataToUniform(UniformMaterial& aUniform, uint64_t aStart, uint64_t aEnd) noexcept {
	if(aStart >= aEnd) {
		std::cout << LogLevel::WARNING << "[GMS] copyDataToUniform start (" << aStart << ") and end (" << aEnd << ") are wrong!\n" << LogLevel::RESET;
		return;
	}

	std::vector<Material> tempDataStore;
	tempDataStore.reserve(aEnd-aStart);

	uint64_t i = 0;
	for(GMSEntry& m : msContainer) {
		if(i < aStart) {
			i++; continue;
		}
		//only standard material
		if(m.variant != GMS_STANDARD_IDENTIFICATOR) {
			i++; continue;
		}
		if(i >= aEnd) { break; }

		if(m.variantData) {
			if(m.variantData->currentVariant != -1) {
				GMSEntry* variant = GlobalMaterialStore::getById(m.variantData->variants[m.variantData->currentVariant]);
				variant->texture.bind(variant->material.textureSlot);
				tempDataStore.push_back(variant->material);
				i++;
				continue;
			}
		}

		m.texture.bind(m.material.textureSlot);
		tempDataStore.push_back(m.material);
		i++;
	}

	//we update buffer - move so starts at 0
	aUniform.update(tempDataStore.data(), 0, aEnd-aStart);
	aUniform.set();
}

GMSEntry* GlobalMaterialStore::getByName(const std::string_view aMaterialName) noexcept {
	for(GMSEntry& m : msContainer) {
		if(m.name == aMaterialName) return &m;
	}
	return nullptr;
}
GMSEntry* GlobalMaterialStore::getByPath(const std::string_view aPath) noexcept {
	for(GMSEntry& m : msContainer) {
		if(m.path == aPath) return &m;
	}
	return nullptr;
}

GMSEntry* GlobalMaterialStore::get(const std::string_view aMaterialName, const std::string_view aVariant) noexcept {
	for(GMSEntry& m : msContainer) {
		if(m.name == aMaterialName && m.variant == aVariant) return &m;
	}
	return nullptr;
}
GMSEntry* GlobalMaterialStore::getStandard(const std::string_view aMaterialName) noexcept {
	for(GMSEntry& m : msContainer) {
		if(m.name == aMaterialName && m.variant == GMS_STANDARD_IDENTIFICATOR) return &m;
	}
	return nullptr;
}

GMSEntry* GlobalMaterialStore::getById(const uint64_t aId) noexcept {
	uint64_t i = 0;
	for(GMSEntry& m : msContainer) {
		if(i == aId) return &m;
		i++;
	}
	return nullptr;
}

uint64_t GlobalMaterialStore::findId(const std::string_view aMaterialName, const std::string_view aVariant) noexcept {
	uint64_t i = 0;
	for(GMSEntry& m : msContainer) {
		if(m.name == aMaterialName && m.variant == aVariant) return i;
		i++;
	}
	return UINT64_MAX;
}

uint64_t GlobalMaterialStore::getLength() noexcept {
	return msContainer.size();
}

GMSEntry* GlobalMaterialStore::addVariant(const std::string_view aMaterialName, const std::string_view aNewVariantName) noexcept {
	GMSEntry* base = getStandard(aMaterialName);

	if(!base) {
		std::cout << LogLevel::WARNING << "[GMS] Cannot make variant for non-existing material!\n" << LogLevel::RESET;
		return nullptr;
	}

	GMSEntry* derived = copyStandard(aMaterialName);
	derived->variant = aNewVariantName;

	if(!base->variantData) {
		msVariantBindings.emplace_back();
		msVariantBindings.back().baseId = findId(aMaterialName);
		msVariantBindings.back().currentVariant = -1;
		base->variantData = &msVariantBindings.back();
	}

	base->variantData->variants.push_back(GlobalMaterialStore::getLength()-1); //last element
	derived->variantData = base->variantData;

	return derived;
}

void GlobalMaterialStore::setVariant(const std::string_view aMaterialName, const std::string_view aIdentificator) noexcept {
	GMSEntry* newVariant = get(aMaterialName, aIdentificator);
	uint64_t id = findId(aMaterialName, aIdentificator);

	for(uint64_t i = 0; i < newVariant->variantData->variants.size(); i++) {
		if(newVariant->variantData->variants[i] == id) {
			newVariant->variantData->currentVariant = i;
			break;
		}
	}
}
void GlobalMaterialStore::resetVariant(const std::string_view aMaterialName) noexcept {
	GMSEntry* variant = getStandard(aMaterialName);
	variant->variantData->currentVariant = -1;
}

std::ostream& GlobalMaterialStore::printData(std::ostream& aStream) noexcept {
	aStream << '{';
	for(GMSEntry& m : msContainer) {
		aStream << m << ',';
	}
	aStream << '}';
	return aStream;
}

GlobalMaterialStore::~GlobalMaterialStore() noexcept {}
GlobalMaterialStore::GlobalMaterialStore() noexcept {}

UniformMaterial::UniformMaterial(const uint64_t aLocation, const uint64_t aAmount) noexcept : StructUniform(aLocation, aAmount) {}
UniformMaterial::~UniformMaterial() noexcept {}
