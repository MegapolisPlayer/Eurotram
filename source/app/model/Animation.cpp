#include "Animation.hpp"

Animation::Animation() noexcept {

}

void Animation::setStateAtFrame(const uint64_t aFrame) noexcept {

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
