#include "Track.hpp"

glm::vec3 Track::getPosition(const float aT) noexcept {
	//find closest points and interpolate between them
	uint64_t id1 = std::floor(aT*(this->pointsAmount-1)); //in range 0 to size-1
	uint64_t id2 = id1 + 1;
	//weight between  the 2 points
	float weight = aT*(this->pointsAmount-1)-id1;

	//for heightpoints
	uint64_t hid1 = std::floor(aT*(TRACK_HEIGHTPOINTS_AMOUNT-1)); //in range 0 to size-1
	uint64_t hid2 = std::ceil(aT*(TRACK_HEIGHTPOINTS_AMOUNT-1));
	float hweight = aT*(TRACK_HEIGHTPOINTS_AMOUNT-1)-hid1;

	glm::vec3 result;

	result.x = Math::linearInterpolation(this->points[id1].x, this->points[id2].x, weight);
	result.y = Math::linearInterpolation(this->heightpoints[hid1], this->heightpoints[hid2], hweight);
	result.z = Math::linearInterpolation(this->points[id1].y, this->points[id2].y, weight);

	return result;
}

glm::vec3 Track::getRotation(const float aT, const bool aTowardFirst) noexcept {
	//vector between position

	//find closest points and interpolate between them
	uint64_t id1 = std::floor(aT*(this->pointsAmount-1)); //in range 0 to size-1
	uint64_t id2 = id1 + 1;

	//for heightpoints
	uint64_t hid1 = std::floor(aT*(TRACK_HEIGHTPOINTS_AMOUNT-1)); //in range 0 to size-1
	uint64_t hid2 = hid1 + 1;

	if(aTowardFirst) {
		std::swap(id1, id2);
		std::swap(hid1, hid2);
	}

	glm::vec3 position = glm::normalize(glm::vec3(
		this->points[id2].x - this->points[id1].x,
		0.0, //height calculated separately (later in func)
		this->points[id2].y - this->points[id1].y
	));

	//rotation

	glm::vec3 result(0.0);
	//https://stackoverflow.com/questions/14066933/direct-way-of-computing-the-clockwise-angle-between-two-vectors determinant formula
	//is made for matrices but work for a pair of vec2s

	//yaw
	glm::vec3 axis = glm::normalize(glm::vec3(0.0, 0.0, 1.0));
	float dotProduct = position.x*axis.x + position.z*axis.z;
	float determinant = position.x*axis.z - position.z*axis.x;
	result.y = glm::degrees(std::atan2(-determinant, -dotProduct)); //rotation around Y axis - yaw

	//pitch
	//height vector: first and second heights - first height (origin 0,0)
	glm::vec2 heightVector = glm::vec2(0, this->heightpoints[hid2]-this->heightpoints[hid1]);

	axis = glm::normalize(glm::vec3(1.0, 0.0, 0.0));
	dotProduct = heightVector.x*axis.x + heightVector.y*axis.y;
	determinant = heightVector.x*axis.y - heightVector.y*axis.x;
	result.z = glm::degrees(std::atan2(-determinant, -dotProduct) + std::numbers::pi); //range 0-360

	//x is empty - no sideways tilt

	return result;
}
