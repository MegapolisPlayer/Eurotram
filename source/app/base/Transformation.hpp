#ifndef EUROTRAM_TRANSFORMATION
#define EUROTRAM_TRANSFORMATION
#include "FreeUtils.hpp"

class Transform {
public:
	Transform() noexcept;

	void setPosition(const glm::vec3& aPosition) noexcept;
	void addPosition(const glm::vec3& aPositionChange) noexcept;
	void resetPosition() noexcept;

	void setRotationX(const float aDegrees) noexcept;
	void setRotationY(const float aDegrees) noexcept;
	void setRotationZ(const float aDegrees) noexcept;

	void addRotationX(const float aDegrees) noexcept;
	void addRotationY(const float aDegrees) noexcept;
	void addRotationZ(const float aDegrees) noexcept;

	void resetRotationX() noexcept;
	void resetRotationY() noexcept;
	void resetRotationZ() noexcept;

	void setScale(const float aScale) noexcept;
	void setScale(const glm::vec3& aScale) noexcept;
	void addScale(const glm::vec3& aPositionChange) noexcept;
	void resetScale() noexcept;

	glm::mat4 getMatrix() const noexcept;
	glm::mat3 getNormalMatrix() const noexcept;

	~Transform() noexcept;
private:
	glm::vec3 mPos;
	float mRotationDegreesX;
	float mRotationDegreesY;
	float mRotationDegreesZ;
	glm::vec3 mScale;

	mutable bool mPrecalculated;
	mutable glm::mat4 mPrecalcMatrix;
	mutable glm::mat3 mPrecalcNormalMatrix;

	void calculateMatrix() const noexcept;
};

#endif
