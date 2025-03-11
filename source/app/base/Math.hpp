#ifndef EUROTRAM_BASE_MATH
#define EUROTRAM_BASE_MATH
#include "FreeUtils.hpp"

namespace Math {
	constexpr uint64_t DEFAULT_BEZIER_PRECISION = 100;

	typedef glm::vec2 BezierPoint;

	float linearInterpolation(const float a1, const float a2, const float aT) noexcept;
	BezierPoint linearInterpolation(const glm::vec2& a1, const glm::vec2& a2, const float aT) noexcept;
	glm::vec3 linearInterpolation(const glm::vec3& a1, const glm::vec3& a2, const float aT) noexcept;

	//we "only" calculate 2D bezier - 3D height is stored in track's heightpoints'
	//aT is between 0 and 1 - state of curve (0.5 - middle point)
	BezierPoint bezierAtPoint(const glm::vec2& aPoint1, const glm::vec2& aCP1, const glm::vec2& aCP2, const glm::vec2& aPoint2, const float aT) noexcept;

	//calculate whole curve - 100 pts
	std::vector<BezierPoint> bezier(const glm::vec2& aPoint1, const glm::vec2& aCP1, const glm::vec2& aCP2, const glm::vec2& aPoint2, const uint64_t aPrecision = DEFAULT_BEZIER_PRECISION) noexcept;

	//fast length of bezier curve
	float bezierLength(const std::vector<glm::vec2>& aPoints) noexcept;

	//get normalized vector by which to move point in curve
	//required for track generation - 2 tracks from single middle bezier
	//this points outward - multiply by -1 to move inward
	glm::vec2 getPerpendicularVectorFromPoints(const glm::vec2& a1, const glm::vec2& a2) noexcept;

	glm::vec2 getAveragePerpendicularVectorFromPoint(const glm::vec2& a1, const glm::vec2& a2, const glm::vec2& a3) noexcept;

	//moves bezier points inward (negative values) or outward (positive values) by specified translation; modifies passed curve
	std::vector<BezierPoint> moveBezier(std::vector<BezierPoint>& aPoints, const glm::vec2& aTranslation) noexcept;

	//calculates normals for plane defined by 4 points
	//front face is GL_CCW (counterclockwise), pass it as counterclockwise
	glm::vec3 normals(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3, glm::vec3 aPoint4) noexcept;

	std::mt19937& getRandomGenerator() noexcept;

	float getRandomNumber(const float aMin, const float aMax) noexcept;

	//returns +1/0/-1 depending on sign
	float getSign(const float aN) noexcept;

	//gets length of track between two points based on straight line length between them
	float getDistance(const glm::vec2& a1, const glm::vec2& a2) noexcept;

	glm::vec2 getAverageOfVectors(const glm::vec2& a1, const glm::vec2& a2) noexcept;
	glm::vec3 getAverageOfVectors(const glm::vec3& a1, const glm::vec3& a2) noexcept;

	float getRotationOfVector2DY(const glm::vec2& aVector) noexcept;
	float getRotationOfVector2DX(const glm::vec2& aVector) noexcept;

	glm::vec2 swizzleXZ(const glm::vec3& aVector) noexcept;
	glm::vec2 swizzleXZ(const glm::vec4& aVector) noexcept;
}

#endif
