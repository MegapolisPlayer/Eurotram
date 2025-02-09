#ifndef EUROTRAM_OBJECTS_LANDMARK
#define EUROTRAM_OBJECTS_LANDMARK
#include "Node.hpp"

struct Landmark {
	RotatedObjectLocation location;
	StationCode landmarkCode;
};

#endif
