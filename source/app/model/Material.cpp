#include "Material.hpp"

UniformMaterial::UniformMaterial(const uint64_t aLocation, const uint64_t aAmount) noexcept : StructUniform(aLocation, aAmount) {}
UniformMaterial::~UniformMaterial() noexcept {}
