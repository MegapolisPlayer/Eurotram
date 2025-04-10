#ifndef EUROTRAM_OBJECTS_TRACK
#define EUROTRAM_OBJECTS_TRACK
#include "Node.hpp"

constexpr uint32_t TRACK_HEIGHTPOINTS_AMOUNT = 10;

enum TrackFlags : uint8_t {
	TRACK_FLAG_FIRST_SWITCH  = 0b00000001,
	TRACK_FLAG_SECOND_SWITCH = 0b00000010,
	TRACK_FLAG_BEZIER        = 0b00000100,
};

struct Track {
	int32_t first, second;
	TrackFlags flags;

	//for bezier
	int32_t point1x, point1y;
	int32_t point2x, point2y;

	int32_t heightpoints[TRACK_HEIGHTPOINTS_AMOUNT];

	StationCode code; //if not empty, is station track

	//generated by map class
	std::vector<glm::vec2> points;
	float length;

	float pointsAmount;

	//get position on track: aT percentage specifies where (0 - start, 0.5 - middle, 1 - end)
	glm::vec3 getPosition(const float aT) noexcept;

	//gets rotation - depends on direction (set bool to true if going towards first node)
	glm::vec3 getRotation(const float aT, const bool aTowardFirst) noexcept;
};

#endif
