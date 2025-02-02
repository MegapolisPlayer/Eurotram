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

struct Skin {
	std::string name;
	uint64_t skinId;
	glm::mat4 inverseBindMatrix;
	std::vector<uint64_t> joints;
	glm::mat4 animation;
	glm::mat4* output;
};

struct SamplerData {
	fastgltf::AnimationPath type = (fastgltf::AnimationPath)0;
	std::vector<glm::vec4> value;
	std::vector<GLfloat> time;
};

class Animation {
	friend class Model;
public:
	Animation() noexcept;

	void setStateAtFrame(const uint64_t aFrame) noexcept;

	~Animation() noexcept;
private:
	std::string mName;

	std::vector<SamplerData> mSamplers;

	float lerp(float aLast, float aNext, float aCurrent) noexcept;
	uint64_t getPositionIndex(const uint64_t aFrame) noexcept;
	uint64_t getRotationIndex(const uint64_t aFrame) noexcept;
	uint64_t getScaleIndex(const uint64_t aFrame) noexcept;
	glm::mat4 interpolatePosition(const uint64_t aFrame) noexcept;
	glm::mat4 interpolateRotation(const uint64_t aFrame) noexcept;
	glm::mat4 interpolateScale(const uint64_t aFrame) noexcept;

	glm::mat4 getBoneTransformations(Skin* aSkin, const uint64_t aFrame) noexcept;
};

#endif
