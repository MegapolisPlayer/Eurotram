#ifndef EUROTRAM_OBJECTS_SWITCH_SIGNAL
#define EUROTRAM_OBJECTS_SWITCH_SIGNAL
#include "Switch.hpp"

struct SwitchSignal {
	RotatedObjectLocation location;
	StationCode code;
	std::string letters;

	Texture switchSignalBoard;
	Framebuffer switchSignalRenderer;

	SwitchSignal() noexcept;
	void updateTexture() noexcept;
};

#endif
