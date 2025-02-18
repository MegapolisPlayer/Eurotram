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

std::ostream& operator<<(std::ostream& aOstream, ObjectLocation& aLocation) noexcept;
std::ostream& operator<<(std::ostream& aOstream, RotatedObjectLocation& aLocation) noexcept;

typedef uint32_t StationCode; //should be 4 8-bit chars - 1 32-bit int.

//accepts 4-char string
StationCode toStationCode(std::string_view aString) noexcept;

struct Node {
	ObjectLocation location;
	StationCode code;
	StationCode codeBorder;
};

#endif
