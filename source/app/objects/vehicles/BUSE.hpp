#ifndef EUROTRAM_OBJECTS_VEHICLES_BUSE
#define EUROTRAM_OBJECTS_VEHICLES_BUSE
#include "../../base/Base.hpp"

enum class BUSEPanelType {
	FRONT = 0, //shows line and end station
	BACK = 1, //shows only line
	SIDE = 2, //shows line, end station and next stop
	INSIDE = 3, //internal large - shows 8 next stops
	SMALL = 4, //internal small - shows next stop

};

class BUSEPanel {
public:
private:
	//BUSEPanelType mType;
};

//TODO add DO NOT ENTER sign at end

#endif
