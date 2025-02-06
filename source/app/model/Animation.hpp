#ifndef EUROTRAM_ANIMATION
#define EUROTRAM_ANIMATION
#include "../base/Base.hpp"

enum struct BoneConditionFilter : uint8_t {
	EXACT_MATCH = 0,
	STARTS_WITH
};

struct BoneCondition {
	std::string_view value;
	BoneConditionFilter filter;

	BoneCondition(std::string_view aValue, BoneConditionFilter aFilter) noexcept : value(aValue), filter(aFilter) {}
};

struct TRSData {
	glm::vec3 t, s;
	glm::quat r;
	fastgltf::AnimationPath type;
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
	void setBoneStateAtTime(Model& aModel, const float aTime, BoneCondition& aBone) noexcept; //for only selected bones

	~Animation() noexcept;
private:
	std::string mName;

	std::vector<SamplerData> mSamplers;

	float lerp(float aLast, float aNext, float aCurrent) noexcept;
	uint64_t getIndex(const SamplerData& aSampler, const float aTime) noexcept;
	glm::vec3 interpolatePosition(const SamplerData& aSampler, const float aTime) noexcept;
	glm::quat interpolateRotation(const SamplerData& aSampler, const float aTime) noexcept;
	glm::vec3 interpolateScale(const SamplerData& aSampler, const float aTime) noexcept;

	TRSData getLocalSamplerTransform(const uint64_t aSamplerId, const float aTime) noexcept;

	static bool fulfills(std::string_view aName, BoneCondition aBone) noexcept;
};

#endif
