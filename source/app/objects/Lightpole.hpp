#ifndef EUROTRAM_OBJECTS_LIGHTPOLE
#define EUROTRAM_OBJECTS_LIGHTPOLE
#include "Node.hpp"

struct Lightpole {
	RotatedObjectLocation location;
	StationCode code;

	Spotlight light;
};

#endif
