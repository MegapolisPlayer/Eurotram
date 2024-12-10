#ifndef EUROTRAM_MODELLOAD
#define EUROTRAM_MODELLOAD
#include "Mesh.hpp"

class Model {
public:
	Model(std::string_view aPath) noexcept;
	~Model();
private:
	static Assimp::Importer smModelLoader;

	std::vector<Mesh> mMeshes;
};

#endif
