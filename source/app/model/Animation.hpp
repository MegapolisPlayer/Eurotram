#ifndef EUROTRAM_ANIMATION
#define EUROTRAM_ANIMATION
#include "../base/Base.hpp"

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
};

class Animation {
	friend class Model;
public:
	Animation() noexcept;

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

	std::string mName;
	uint64_t mTickAmount;
	uint64_t mTicksPerSecond;
};

#endif
