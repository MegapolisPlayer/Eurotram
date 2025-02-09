//TODO objects have foundations
//TODO instance objects!!!!!!!!

#ifndef EUROTRAM_OBJECTS_BUILDING
#define EUROTRAM_OBJECTS_BUILDING
#include "Node.hpp"

enum struct BuildingType : uint8_t {
	NORMAL = 0, //normalni radovy
	CORNER, //uhlovy
	FAMILY, //rodinny
	PREFAB, //panelak
};

struct Building {
	RotatedObjectLocation location;
	BuildingType type;
};

#endif
