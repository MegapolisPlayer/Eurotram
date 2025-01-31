#include "Animation.hpp"

Animation::Animation() noexcept : mTickAmount(0), mTicksPerSecond(0) {}

void Animation::addBone(Bone* aBone) noexcept {
	this->mBonesRef.push_back(aBone);
}
void Animation::addBones(std::vector<Bone*>& aBones) noexcept {
	this->mBonesRef.reserve(aBones.size());
	for(Bone* b :aBones) this->mBonesRef.push_back(b);
}

void Animation::setStateAtFrame(const uint64_t aFrame) noexcept {
	for(Bone* b : this->mBonesRef) {
		//for each bone - update matrix
		*(b->transformation) = getBoneTransformation(b, aFrame%this->mTickAmount);
	}
}

uint64_t Animation::getTickAmount() const noexcept {
	return this->mTickAmount;
}
uint64_t Animation::getTicksPerSecond() const noexcept {
	return this->mTicksPerSecond;
}

float Animation::getAnimationTime(const uint64_t aFrame) const noexcept {
	return (float)aFrame/(float)this->mTicksPerSecond;
}

std::vector<Keyframe::Position>& Animation::getPositionKeyframes() noexcept {
	return this->mPositions;
}
std::vector<Keyframe::Rotation>& Animation::getRotationKeyframes() noexcept {
	return this->mRotation;
}
std::vector<Keyframe::Scale>& Animation::getScaleKeyframes() noexcept {
	return this->mScale;
}

Animation::~Animation() noexcept {}

glm::mat4 Animation::getBoneTransformation(Bone* aBone, const uint64_t aFrame) noexcept {
	glm::mat4 result = glm::mat4(1.0f);
	Bone* tempBone = aBone;

	while(tempBone != nullptr) {
		//we go from bottom to top - reverse matrix multiplication order!
		result = result * tempBone->offset;
		std::cout << ">" << tempBone->name;
		tempBone = tempBone->parent;
	}
	std::cout << "\n";

	return result;
}

//TODO finish animation

uint64_t Animation::getPositionIndex(const uint64_t aFrame) noexcept {
	return {};
}
uint64_t Animation::getRotationIndex(const uint64_t aFrame) noexcept {
	return {};
}
uint64_t Animation::getScaleIndex(const uint64_t aFrame) noexcept {
	return {};
}

//linear interpolation
float Animation::getScaleFactor(const uint64_t aLastFrame, const uint64_t aNextFrame, const uint64_t aCurrentFrame) noexcept {
	return {};
}

glm::mat4 Animation::interpolatePosition(const uint64_t aFrame) noexcept {
	int p0Index = getPositionIndex(aFrame);
	int p1Index = p0Index + 1;
	float scale = getScaleFactor(this->mPositions[p0Index].frame, this->mPositions[p1Index].frame, aFrame);
	glm::vec3 position = glm::mix(this->mPositions[p0Index].position, this->mPositions[p1Index].position, scale);
	return glm::translate(glm::mat4(1.0f), position);
}
glm::mat4 Animation::interpolateRotation(const uint64_t aFrame) noexcept {
	return {};
}
glm::mat4 Animation::interpolateScale(const uint64_t aFrame) noexcept {
	return {};
}
