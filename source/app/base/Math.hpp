#ifndef EUROTRAM_BASE_MATH
#define EUROTRAM_BASE_MATH
#include "FreeUtils.hpp"

namespace Math {
	constexpr uint64_t DEFAULT_BEZIER_PRECISION = 100;

	typedef glm::vec2 BezierPoint;

	BezierPoint linearInterpolation(const glm::vec2& a1, const glm::vec2& a2, const float aT) noexcept;

	//we "only" calculate 2D bezier - 3D height is stored in track's heightpoints'
	//aT is between 0 and 1 - state of curve (0.5 - middle point)
	BezierPoint bezierAtPoint(const glm::vec2& aPoint1, const glm::vec2& aCP1, const glm::vec2& aCP2, const glm::vec2& aPoint2, const float aT) noexcept;

	//calculate whole curve - 100 pts
	std::vector<BezierPoint> bezier(const glm::vec2& aPoint1, const glm::vec2& aCP1, const glm::vec2& aCP2, const glm::vec2& aPoint2, const uint64_t aPrecision = DEFAULT_BEZIER_PRECISION) noexcept;

	//fast length of bezier curve
	float bezierLength(std::vector<glm::vec2> aPoints) noexcept;

	//get normalized vector by which to move point in curve
	//required for track generation - 2 tracks from single middle bezier
	//this points outward - multiply by -1 to move inward
	glm::vec2 getBezierPerpendicularVector(const glm::vec2& a1, const glm::vec2& a2, const glm::vec3& a3) noexcept;

	//calculates normals for plane defined by 4 points
	glm::vec3 normals(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3, glm::vec3 aPoint4) noexcept;
}

#endif
