#include "Transformation.hpp"

Transform::Transform() noexcept
	: mPos(0.0f), mRotationDegreesX(1.0f), mRotationDegreesY(1.0f), mRotationDegreesZ(1.0f),
		mScale(1.0f), mPrecalculated(false), mPrecalcMatrix(1.0f), mPrecalcNormalMatrix(1.0f) {}

Transform::Transform(const glm::mat4& aMatrix) noexcept
	: mPos(0.0f), mRotationDegreesX(1.0f), mRotationDegreesY(1.0f), mRotationDegreesZ(1.0f),
		mScale(1.0f), mPrecalculated(true), mPrecalcMatrix(aMatrix), mPrecalcNormalMatrix(1.0f) {
	glm::quat rotation;
	glm::vec3 skew;
	glm::vec4 perspective;
	//decompose so class is happy
	glm::decompose(this->mPrecalcMatrix, this->mScale, rotation, this->mPos, skew, perspective);
	glm::vec3 rotationAngles = glm::eulerAngles(glm::conjugate(rotation)); //conjugate is inverse of quaternion
	this->mRotationDegreesX = rotationAngles.x;
	this->mRotationDegreesY = rotationAngles.y;
	this->mRotationDegreesZ = rotationAngles.z;

	this->calculateMatrix();
}

Transform::Transform(Transform& aOther) noexcept {
	this->mPos = aOther.mPos;
	this->mRotationDegreesX = aOther.mRotationDegreesX;
	this->mRotationDegreesY = aOther.mRotationDegreesY;
	this->mRotationDegreesZ = aOther.mRotationDegreesZ;
	this->mScale = aOther.mScale;
	this->mPrecalculated = aOther.mPrecalculated;
	this->mPrecalcMatrix = aOther.mPrecalcMatrix;
	this->mPrecalcNormalMatrix = aOther.mPrecalcNormalMatrix;
}
Transform::Transform(Transform&& aOther) noexcept {
	this->mPos = std::move(aOther.mPos);
	this->mRotationDegreesX = std::move(aOther.mRotationDegreesX);
	this->mRotationDegreesY = std::move(aOther.mRotationDegreesY);
	this->mRotationDegreesZ = std::move(aOther.mRotationDegreesZ);
	this->mScale = std::move(aOther.mScale);
	this->mPrecalculated = std::move(aOther.mPrecalculated);
	this->mPrecalcMatrix = std::move(aOther.mPrecalcMatrix);
	this->mPrecalcNormalMatrix = std::move(aOther.mPrecalcNormalMatrix);
}
Transform& Transform::operator=(Transform& aOther) noexcept {
	this->mPos = aOther.mPos;
	this->mRotationDegreesX = aOther.mRotationDegreesX;
	this->mRotationDegreesY = aOther.mRotationDegreesY;
	this->mRotationDegreesZ = aOther.mRotationDegreesZ;
	this->mScale = aOther.mScale;
	this->mPrecalculated = aOther.mPrecalculated;
	this->mPrecalcMatrix = aOther.mPrecalcMatrix;
	this->mPrecalcNormalMatrix = aOther.mPrecalcNormalMatrix;
	return *this;
}
Transform& Transform::operator=(Transform&& aOther) noexcept {
	this->mPos = std::move(aOther.mPos);
	this->mRotationDegreesX = std::move(aOther.mRotationDegreesX);
	this->mRotationDegreesY = std::move(aOther.mRotationDegreesY);
	this->mRotationDegreesZ = std::move(aOther.mRotationDegreesZ);
	this->mScale = std::move(aOther.mScale);
	this->mPrecalculated = std::move(aOther.mPrecalculated);
	this->mPrecalcMatrix = std::move(aOther.mPrecalcMatrix);
	this->mPrecalcNormalMatrix = std::move(aOther.mPrecalcNormalMatrix);
	return *this;
}

void Transform::setPosition(const glm::vec3& aPosition) noexcept {
	this->mPos = aPosition;
	this->mPrecalculated = false;
}
void Transform::addPosition(const glm::vec3& aPositionChange) noexcept {
	this->mPos += aPositionChange;
	this->mPrecalculated = false;
}
void Transform::resetPosition() noexcept {
	this->mPos = glm::vec3(1.0f);
	this->mPrecalculated = false;
}

void Transform::setRotationX(const float aDegrees) noexcept {
	this->mRotationDegreesX = aDegrees;
	this->mPrecalculated = false;
}
void Transform::setRotationY(const float aDegrees) noexcept {
	this->mRotationDegreesY = aDegrees;
	this->mPrecalculated = false;
}
void Transform::setRotationZ(const float aDegrees) noexcept {
	this->mRotationDegreesZ = aDegrees;
	this->mPrecalculated = false;
}
void Transform::setRotation(const glm::vec3& aEuler) noexcept {
	this->mRotationDegreesX = aEuler.x;
	this->mRotationDegreesY = aEuler.y;
	this->mRotationDegreesZ = aEuler.z;
	this->mPrecalculated = false;
}

void Transform::addRotationX(const float aDegrees) noexcept {
	this->mRotationDegreesX += aDegrees;
	this->mPrecalculated = false;
}
void Transform::addRotationY(const float aDegrees) noexcept {
	this->mRotationDegreesY += aDegrees;
	this->mPrecalculated = false;
}
void Transform::addRotationZ(const float aDegrees) noexcept {
	this->mRotationDegreesZ += aDegrees;
	this->mPrecalculated = false;
}
void Transform::resetRotationX() noexcept {
	this->mRotationDegreesX = 0.0f;
	this->mPrecalculated = false;
}
void Transform::resetRotationY() noexcept {
	this->mRotationDegreesY = 0.0f;
	this->mPrecalculated = false;
}
void Transform::resetRotationZ() noexcept {
	this->mRotationDegreesZ = 0.0f;
	this->mPrecalculated = false;
}

void Transform::setScale(const float aScale) noexcept {
	this->mScale = glm::vec3(aScale);
	this->mPrecalculated = false;
}
void Transform::setScale(const glm::vec3& aScale) noexcept {
	this->mScale = aScale;
	this->mPrecalculated = false;
}
void Transform::addScale(const glm::vec3& aPositionChange) noexcept {
	this->mScale = aPositionChange;
	this->mPrecalculated = false;
}
void Transform::resetScale() noexcept {
	this->mScale = glm::vec3(1.0f);
	this->mPrecalculated = false;
}

glm::mat4 Transform::getMatrix() const noexcept {
	this->calculateMatrix();
	return this->mPrecalcMatrix;
}
glm::mat3 Transform::getNormalMatrix() const noexcept {
	this->calculateMatrix();
	return this->mPrecalcNormalMatrix;
}
bool Transform::isUpdated() const noexcept {
	return this->mPrecalculated;
}

Transform::~Transform() noexcept {}

void Transform::calculateMatrix() const noexcept {
	if(this->mPrecalculated) return;

	//TRS
	//first multiply scale, then rot, then translate

	this->mPrecalcMatrix = glm::mat4(1.0f);

	this->mPrecalcMatrix = glm::translate(this->mPrecalcMatrix, this->mPos);


	this->mPrecalcMatrix = glm::rotate(
		this->mPrecalcMatrix, glm::radians(this->mRotationDegreesX),
		glm::vec3(1.0f, 0.0f, 0.0f)
	);
	this->mPrecalcMatrix = glm::rotate(
		this->mPrecalcMatrix, glm::radians(this->mRotationDegreesY),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	this->mPrecalcMatrix = glm::rotate(
		this->mPrecalcMatrix, glm::radians(this->mRotationDegreesZ),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	this->mPrecalcMatrix = glm::scale(
		this->mPrecalcMatrix, this->mScale
	);

	this->mPrecalcNormalMatrix = glm::transpose(glm::inverse(this->mPrecalcMatrix));

	this->mPrecalculated = true;
}
