#ifndef EUROTRAM_ANIMATION
#define EUROTRAM_ANIMATION
#include "../base/Base.hpp"

struct TRSData {
	glm::mat4 t, r, s;
};

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
	std::vector<uint64_t> joints;
};

struct SamplerData {
	fastgltf::AnimationPath type = (fastgltf::AnimationPath)0;
	std::vector<glm::vec4> value;
	std::vector<GLfloat> time;
	int64_t nodeIndex;

	SamplerData() noexcept : nodeIndex(-1) {}
	SamplerData(const fastgltf::AnimationPath aType, std::vector<glm::vec4>& aValue, std::vector<GLfloat>& aTime, const int64_t aNodeIndex) noexcept
		:type(aType), value(aValue), time(aTime), nodeIndex(aNodeIndex) {}
	~SamplerData() {}
};

class Model;

class Animation {
	friend class Model;
public:
	Animation() noexcept;

	void setStateAtTime(Model& aModel, const float aTime) noexcept;

	~Animation() noexcept;
private:
	std::string mName;

	std::vector<SamplerData> mSamplers;

	float lerp(float aLast, float aNext, float aCurrent) noexcept;
	uint64_t getIndex(const SamplerData& aSampler, const float aTime) noexcept;
	glm::mat4 interpolatePosition(const SamplerData& aSampler, const float aTime) noexcept;
	glm::mat4 interpolateRotation(const SamplerData& aSampler, const float aTime) noexcept;
	glm::mat4 interpolateScale(const SamplerData& aSampler, const float aTime) noexcept;

	void getBoneTransformations(Model& aModel, const uint64_t aSamplerId, const float aTime, std::vector<TRSData>& aTRSData) noexcept;
};

#endif
