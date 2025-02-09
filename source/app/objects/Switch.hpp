#ifndef EUROTRAM_OBJECTS_SWITCH
#define EUROTRAM_OBJECTS_SWITCH
#include "Node.hpp"
#include "Trigger.hpp"

struct Switch {
	ObjectLocation location;
	StationCode code;
	uint32_t before, left, front, right, radioBox, signal;
};

#endif
