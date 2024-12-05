#include "Lighting.hpp"

UniformDirlight::UniformDirlight(const uint64_t aLocation) noexcept : StructUniform(aLocation) {}
UniformDirlight::~UniformDirlight() noexcept {}
UniformPointlight::UniformPointlight(const uint64_t aLocation) noexcept : StructUniform(aLocation) {}
UniformPointlight::~UniformPointlight() noexcept {}
UniformSpotlight::UniformSpotlight(const uint64_t aLocation) noexcept : StructUniform(aLocation) {}
UniformSpotlight::~UniformSpotlight() noexcept {}
