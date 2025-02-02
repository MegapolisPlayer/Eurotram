#include "Animation.hpp"

Animation::Animation() noexcept  {}

void Animation::setStateAtFrame(const uint64_t aFrame) noexcept {

}

Animation::~Animation() noexcept {}

//TODO calculate this
//jointMatrix[j] = inverse(globalTransform) * globalJointTransform[j] * inverseBindMatrix[j]
//https://github.com/KhronosGroup/glTF/issues/1630
//jointMatrix[j] = inverse(localNodeTransform) * worldTransform[j] * inverseBindMatrix[j]
//localNodeTransform - from node
//worldTransform -- also from node
//
//getting which bone -> node can reference skin (node.second - the id of the "skin")

float Animation::lerp(float aLast, float aNext, float aCurrent) noexcept {
	return ((float)aCurrent - (float)aLast)/((float)aNext - (float)aLast); //should be in range 0-1
}
uint64_t Animation::getPositionIndex(const uint64_t aFrame) noexcept {
	return {}; //TODO
}
uint64_t Animation::getRotationIndex(const uint64_t aFrame) noexcept {
	return {}; //TODO
}
uint64_t Animation::getScaleIndex(const uint64_t aFrame) noexcept {
	return {}; //TODO
}
glm::mat4 Animation::interpolatePosition(const uint64_t aFrame) noexcept {
	return {}; //TODO
}
glm::mat4 Animation::interpolateRotation(const uint64_t aFrame) noexcept {
	return {}; //TODO
}
glm::mat4 Animation::interpolateScale(const uint64_t aFrame) noexcept {
	return {}; //TODO
}

glm::mat4 Animation::getBoneTransformations(Skin* aSkin, const uint64_t aFrame) noexcept {
	//Joint* tempBone = aBone;
	glm::mat4 result = glm::mat4(1.0f);

	std::cout << "Resulting transform from world origin for " <<  ": " << (result * glm::vec4(1.0f)) << "\n";

	return result;
}
