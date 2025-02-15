#ifndef EUROTRAM_OBJECTS_SWITCH
#define EUROTRAM_OBJECTS_SWITCH
#include "Node.hpp"
#include "Trigger.hpp"

struct Switch {
	ObjectLocation location;
	StationCode code;
	//1 if not set
	int32_t before, left, front, right, radioBox, signal;
	unsigned char letter;
};

#endif
