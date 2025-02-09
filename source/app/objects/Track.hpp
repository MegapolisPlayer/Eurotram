#ifndef EUROTRAM_OBJECTS_TRACK
#define EUROTRAM_OBJECTS_TRACK
#include "Node.hpp"

constexpr uint32_t TRACK_HEIGHTPOINTS_AMOUNT = 10;

enum TRACK_FLAGS : uint8_t {
	TRACK_FLAG_FIRST_SWITCH  = 0b00000001,
	TRACK_FLAG_SECOND_SWITCH = 0b00000010,
	TRACK_FLAG_BEZIER        = 0b00000100,
};

struct Track {
	uint32_t first, second;
	TRACK_FLAGS flags;

	//for bezier
	uint32_t point1x, point1y;
	uint32_t point2x, point2y;

	uint32_t heightpoints[TRACK_HEIGHTPOINTS_AMOUNT];

	StationCode code; //if not empty, is station track
};

#endif
