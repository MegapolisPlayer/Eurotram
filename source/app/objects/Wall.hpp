#ifndef EUROTRAM_OBJECTS_WALL
#define EUROTRAM_OBJECTS_WALL
#include "Node.hpp"

struct Wall {
	ObjectLocation firstPoint;
	ObjectLocation secondPoint;
	uint64_t height;
};

#endif
