#include "Node.hpp"

std::ostream& operator<<(std::ostream& aOstream, ObjectLocation& aLocation) noexcept {
	aOstream << "{X=" << aLocation.x << ",Y=" << aLocation.y << ",H=" << aLocation.h << '}';
	return aOstream;
}
std::ostream& operator<<(std::ostream& aOstream, RotatedObjectLocation& aLocation) noexcept {
	aOstream << "{X=" << aLocation.x << ",Y=" << aLocation.y << ",H=" << aLocation.h << ",R=" << aLocation.r << '}';
	return aOstream;
}

StationCode toStationCode(std::string_view aString) noexcept {
	char stationCodeData[4] = {0,0,0,0};
	uint8_t amount = std::min(4ul, aString.size());
	for(uint8_t i = 0; i < amount; i++) {
		stationCodeData[i] = aString[i];
	}

	return *((uint32_t*)stationCodeData);
}
