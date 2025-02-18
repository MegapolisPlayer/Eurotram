#ifndef EUROTRAM_OBJECTS_SIGNAL
#define EUROTRAM_OBJECTS_SIGNAL
#include "Switch.hpp"

struct Signal {
	RotatedObjectLocation location;
	uint32_t poleHeight; //if 0 - hang on overhead wire
	StationCode code;

	int32_t signalCopyId; //if not -1 -> is presignal, copies data from that id

	Texture signalBoard;
	Framebuffer signalRenderer;

	Signal() noexcept;
	void updateTexture() noexcept;
};

#endif
