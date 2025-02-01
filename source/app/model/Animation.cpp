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
	std::cout << "Animation " << this->mName << "; TPS: " << this->mTicksPerSecond << "; ticks: " << this->mTickAmount << '\n';

	for(Bone* b : this->mBonesRef) {
		std::cout << b->name << " ";
		if(b->parent != nullptr) {
			std::cout << "(parent " << b->parent->name << ')';
		}
		std::cout << '\n';
	}

	for(Bone* b : this->mBonesRef) {
		*(b->transformation) = getBoneTransformation(b, aFrame);
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
	Bone* tempBone = aBone;
	std::vector<Bone*> path;

	//make path from list of parents
	while(tempBone != nullptr) {
		path.push_back(tempBone);
		tempBone = tempBone->parent;
	}

	//next element child of previous

	glm::mat4 result = glm::mat4(1.0f);
	for(uint64_t i = 0; i < path.size(); i++) {
		path[i]->local = interpolatePosition(aFrame) * interpolateRotation(aFrame) * interpolateScale(aFrame);
		if(i >= 1) {
			result *= glm::inverse(path[i-1]->offset);
		}
		result *= path[i]->offset;
		result *= path[i]->local;
	}

	std::cout << "Resulting transform from world origin for " << aBone->name << ": " << (result * glm::vec4(1.0f)) << "\n";

	std::cout << "\n";

	return result;
}

uint64_t Animation::getPositionIndex(const uint64_t aFrame) noexcept {
	for(uint64_t i = 0; i < this->mPositions.size()-1; i++) {
		if(this->mPositions[i+1].frame > aFrame) return i;
	}
	return UINT64_MAX;
}
uint64_t Animation::getRotationIndex(const uint64_t aFrame) noexcept {
	for(uint64_t i = 0; i < this->mRotation.size()-1; i++) {
		if(this->mRotation[i+1].frame > aFrame) return i;
	}
	return UINT64_MAX;
}
uint64_t Animation::getScaleIndex(const uint64_t aFrame) noexcept {
	for(uint64_t i = 0; i < this->mScale.size()-1; i++) {
		if(this->mScale[i+1].frame > aFrame) return i;
	}
	return UINT64_MAX;
}

//linear interpolation - lerp
float Animation::lerp(const uint64_t aLastFrame, const uint64_t aNextFrame, const uint64_t aCurrentFrame) noexcept {
	std::cout << "lerp - LF " << aLastFrame << " NF " << aNextFrame << " CF " << aCurrentFrame << '\n';
	return ((float)aCurrentFrame - (float)aLastFrame)/((float)aNextFrame - (float)aLastFrame); //should be in range 0-1
}

glm::mat4 Animation::interpolatePosition(const uint64_t aFrame) noexcept {
	if(this->mPositions.size() == 1) {
		std::cout << "no pos!\n";
		return glm::translate(glm::mat4(1.0f), this->mPositions[0].position);
	}

	uint64_t start = getPositionIndex(aFrame);
	uint64_t end = start + 1;
	float weight = this->lerp(this->mPositions[start].frame, this->mPositions[end].frame, aFrame);
	std::cout << "pos from " << this->mPositions[start].position << '\n';
	std::cout << "pos to " << this->mPositions[end].position << '\n';
	std::cout << "pos weight " << weight << '\n';
	glm::vec3 pos = glm::mix(this->mPositions[start].position, this->mPositions[end].position, weight);
	std::cout << "pos " << pos << '\n';
	return glm::translate(glm::mat4(1.0f), pos);
}
glm::mat4 Animation::interpolateRotation(const uint64_t aFrame) noexcept {
	if(this->mRotation.size() == 1) {
		std::cout << "no rot!\n";
		return glm::mat4_cast(glm::normalize(this->mRotation[0].rotation));
	}

	uint64_t start = getPositionIndex(aFrame);
	uint64_t end = start + 1;
	float weight = this->lerp(this->mRotation[start].frame, this->mRotation[end].frame, aFrame);
	std::cout << "rot weight " << weight << '\n';

	//slerp - lerp for rotations
	//get quaternion - normalize - cast to matrix
	return glm::mat4_cast(glm::normalize(glm::slerp(this->mRotation[start].rotation, this->mRotation[end].rotation, weight)));
}
glm::mat4 Animation::interpolateScale(const uint64_t aFrame) noexcept {
	if(this->mScale.size() == 1) {
		std::cout << "no scale!\n";
		return glm::scale(glm::mat4(1.0f), this->mScale[0].scale);
	}

	uint64_t start = getPositionIndex(aFrame);
	uint64_t end = start + 1;
	float weight = this->lerp(this->mScale[start].frame, this->mScale[end].frame, aFrame);
	std::cout << "scale from " << this->mScale[start].scale << '\n';
	std::cout << "scale to " << this->mScale[end].scale << '\n';
	std::cout << "scale weight " << weight << '\n';
	glm::vec3 scale = glm::mix(this->mScale[start].scale, this->mScale[end].scale, weight);
	std::cout << "scale " << scale << '\n';
	return glm::scale(glm::mat4(1.0f), scale);
}
