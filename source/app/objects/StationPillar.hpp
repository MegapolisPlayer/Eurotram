#ifndef EUROTRAM_OBJECTS_STATION_PILLAR
#define EUROTRAM_OBJECTS_STATION_PILLAR
#include "Node.hpp"

struct StationPillar {
	RotatedObjectLocation location;
	StationCode code;

	Texture pillarBoard; //"line board" - shows which lines call at station
	Framebuffer pillarRenderer;

	StationPillar() noexcept;

	void updateTexture() noexcept;
};

#endif
