#ifndef EUROTRAM_ANIMATION
#define EUROTRAM_ANIMATION
#include "../base/Base.hpp"

//assimp is weird and loads like 3333 frames - solution to just hard-code it
constexpr double ANIMATION_FRAME_PER_FRAME_AMOUNT = 41.6625;

namespace Keyframe {
	struct Position {
		glm::vec3 position;
		uint64_t frame = 0;
	};
	struct Rotation {
		glm::quat rotation; //quaternions...
		uint64_t frame = 0;
	};
	struct Scale {
		glm::vec3 scale;
		uint64_t frame = 0;
	};
}

struct Bone {
	std::string name;
	glm::mat4 offset;
	glm::mat4 local;
	glm::mat4* transformation; //write data to pointer
	Bone* parent;
};

class Animation {
	friend class Model;
public:
	Animation() noexcept;

	void addBone(Bone* aBone) noexcept;
	void addBones(std::vector<Bone*>& aBones) noexcept;

	void setStateAtFrame(const uint64_t aFrame) noexcept;

	uint64_t getTickAmount() const noexcept;
	uint64_t getTicksPerSecond() const noexcept;

	float getAnimationTime(const uint64_t aFrame) const noexcept; //for interpolation

	std::vector<Keyframe::Position>& getPositionKeyframes() noexcept;
	std::vector<Keyframe::Rotation>& getRotationKeyframes() noexcept;
	std::vector<Keyframe::Scale>& getScaleKeyframes() noexcept;

	~Animation() noexcept;
private:
	std::vector<Keyframe::Position> mPositions;
	std::vector<Keyframe::Rotation> mRotation;
	std::vector<Keyframe::Scale> mScale;

	std::vector<Bone*> mBonesRef;

	std::string mName;
	uint64_t mTickAmount;
	uint64_t mTicksPerSecond;

	glm::mat4 getBoneTransformation(Bone* aBone, const uint64_t aFrame) noexcept;

	uint64_t getPositionIndex(const uint64_t aFrame) noexcept;
	uint64_t getRotationIndex(const uint64_t aFrame) noexcept;
	uint64_t getScaleIndex(const uint64_t aFrame) noexcept;

	//interpolate between last and next frame - get value at current frame
	float lerp(const uint64_t aLastFrame, const uint64_t aNextFrame, const uint64_t aCurrentFrame) noexcept;

	glm::mat4 interpolatePosition(const uint64_t aFrame) noexcept;
	glm::mat4 interpolateRotation(const uint64_t aFrame) noexcept;
	glm::mat4 interpolateScale(const uint64_t aFrame) noexcept;
};

#endif
