#ifndef EUROTRAM_OBJECTS_VEHICLES_BUSE
#define EUROTRAM_OBJECTS_VEHICLES_BUSE
#include "../../base/Base.hpp"

enum class BUSEPanelType : uint8_t {
	FRONT = 0, //shows line and end station
	BACK = 1, //shows only line
	SIDE = 2, //shows line, end station and next stop
	INSIDE = 3, //internal large - shows 8 next stops
	SMALL = 4, //internal small - shows next stop

};

class BUSEPanel {
public:
	BUSEPanel(BUSEPanelType aType) noexcept;

	void draw(std::string_view aLine, std::string_view aNextStop, std::string_view aEndStop) noexcept;
	//TODO overload which takes vector of string views - for large panels

	~BUSEPanel() noexcept;
private:
	BUSEPanelType mType;
};

//TODO add DO NOT ENTER sign at end

#endif
