//TODO event system

//event types - RADIOBOX_SIGNAL, SWITCH_TO_SIGNAL, ...
//PAYLOAD - set as chunk of memory with size

//event dispatcher -> event queue -> event listener
//events non-blocking - every frame processes them
//

#ifndef EUROTRAM_EVENT
#define EUROTRAM_EVENT
#include "FreeUtils.hpp"

namespace EventType {
	enum EventType : uint64_t {
		UNDEFINED = 0,
		STATION_ARRIVAL,
		RADIOBOX_UPDATE,
		SWITCH_SIGNAL_UPDATE,
	};
};

#endif
