#include "Material.hpp"

GMSEntry::GMSEntry() noexcept : texture("") {}

std::list<GMSEntry> GlobalMaterialStore::msContainer;

GMSEntry* GlobalMaterialStore::add() noexcept {
	msContainer.emplace_back();
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

uint64_t GlobalMaterialStore::getLength() noexcept {
	return msContainer.size();
}

GlobalMaterialStore::~GlobalMaterialStore() noexcept {}
GlobalMaterialStore::GlobalMaterialStore() noexcept {}

UniformMaterial::UniformMaterial(const uint64_t aLocation, const uint64_t aAmount) noexcept : StructUniform(aLocation, aAmount) {}
UniformMaterial::~UniformMaterial() noexcept {}
