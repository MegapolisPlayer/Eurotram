#include "Math.hpp"

namespace Math {
	//3x faster than using glm::mix
	BezierPoint linearInterpolation(const glm::vec2& a1, const glm::vec2& a2, const float aT) noexcept {
		return glm::vec2(a1.x+((a2.x-a1.x)*aT), a1.y+((a2.y-a1.y)*aT));
	}

	BezierPoint bezierAtPoint(const glm::vec2& aPoint1, const glm::vec2& aCP1, const glm::vec2& aCP2, const glm::vec2& aPoint2, const float aT) noexcept {
		//https://upload.wikimedia.org/wikipedia/commons/d/db/B%C3%A9zier_3_big.gif demonstration gif

		//we calculate "middle points" of lines between original (3 points, 2 lines - "green" ones)
		//we then calculate "middle points" of those lines (2 points, 1 line - "blue one")
		//middle of this - resulting curve

		glm::vec2 firstBasePoint = linearInterpolation(aPoint1, aCP1, aT);
		glm::vec2 secondBasePoint = linearInterpolation(aCP1, aCP2, aT);
		glm::vec2 thirdBasePoint = linearInterpolation(aCP2, aPoint2, aT);

		glm::vec2 firstImprovedPoint = linearInterpolation(firstBasePoint, secondBasePoint, aT);
		glm::vec2 secondImprovedPoint = linearInterpolation(secondBasePoint, thirdBasePoint, aT);

		return linearInterpolation(firstImprovedPoint, secondImprovedPoint, aT);
	}

	std::vector<BezierPoint> bezier(const glm::vec2& aPoint1, const glm::vec2& aCP1, const glm::vec2& aCP2, const glm::vec2& aPoint2, const uint64_t aPrecision) noexcept {
		std::vector<BezierPoint> result;
		result.reserve(aPrecision);
		for(uint64_t i = 0; i < aPrecision; i++) {
			result.push_back(bezierAtPoint(aPoint1, aCP1, aCP2, aPoint2, i*(1.0/aPrecision)));
		}
		return result;
	}

	float bezierLength(const std::vector<glm::vec2>& aPoints) noexcept {
		//simplest thing - calculate distances between points (pythagorean theorem)
		float result = 0.0;

		//calculate for this and next point, ignore last point in array
		for(uint64_t i = 0; i < aPoints.size()-2; i++) {

			//simple pythagorean theorem - half the time from glm::distance
			result += std::sqrt(
				std::pow(aPoints[i+1].x-aPoints[i].x, 2) + std::pow(aPoints[i+1].y-aPoints[i].y, 2)
			);
		}

		return result;
	}

	glm::vec2 getPerpendicularVectorFromPoints(const glm::vec2& a1, const glm::vec2& a2) noexcept {
		glm::vec2 directionVector = a2 - a1;
		//[-y, x] rotates vector by 90 degrees
		return glm::normalize(glm::vec2(-directionVector.y, directionVector.x));
	}

	glm::vec2 getAveragePerpendicularVectorFromPoint(const glm::vec2& a1, const glm::vec2& a2, const glm::vec2& a3) noexcept {
		glm::vec2 a = getPerpendicularVectorFromPoints(a1, a2);
		glm::vec2 b = getPerpendicularVectorFromPoints(a2, a3);
		return (a + b) / glm::vec2(2.0); //average of normalized vectors still normalized vector
	}

	std::vector<BezierPoint> moveBezier(std::vector<BezierPoint>& aPoints, const glm::vec2& aTranslation) noexcept {
		std::vector<glm::vec2> resultingCurve;
		resultingCurve.reserve(aPoints.size());

		//first point - no average
		resultingCurve.push_back(aPoints[0]+getPerpendicularVectorFromPoints(aPoints[0], aPoints[1]));

		for(uint64_t i = 1; i < aPoints.size()-1; i++) {
			glm::vec2 direction = getAveragePerpendicularVectorFromPoint(aPoints[i-1], aPoints[i], aPoints[i+1]);
			resultingCurve.push_back(aPoints[i] + direction*aTranslation);
		}

		resultingCurve.push_back(aPoints[aPoints.size()-1]+getPerpendicularVectorFromPoints(aPoints[aPoints.size()-2], aPoints[aPoints.size()-1]));

		return resultingCurve;
	}

	//cross product (vektorovy soucin) to 2 non-parallel edges of polygon
	glm::vec3 normals(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3, glm::vec3 aPoint4) noexcept {
		DISCARD(aPoint4);
		glm::vec3 edge1 = aPoint2 - aPoint1;
		glm::vec3 edge2 = aPoint3 - aPoint2;
		return glm::cross(edge1, edge2);
	}
}
