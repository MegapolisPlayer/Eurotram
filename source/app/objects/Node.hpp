#ifndef EUROTRAM_OBJECT_NODE
#define EUROTRAM_OBJECT_NODE
#include "../model/Model.hpp"

//type definitions
struct ObjectLocation {
	float x, y, h;
};

struct RotatedObjectLocation : public ObjectLocation {
	float r;
};

typedef uint32_t StationCode; //should be 4 8-bit chars - 1 32-bit int.

struct Node {
	ObjectLocation location;
	StationCode code;
};

#endif
