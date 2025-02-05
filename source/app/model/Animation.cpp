#include "Animation.hpp"

#include "ModelLoading.hpp" //so model class works

Animation::Animation() noexcept  {}

void Animation::setStateAtTime(Model& aModel, const float aTime) noexcept {
	//only calc and update local matrices of nodes
	//rest done in model class

	std::vector<TRSData> data;
	data.resize(aModel.mNodes.size());
	for(uint64_t i = 0; i < this->mSamplers.size(); i++) {
		this->getLocalSamplerTransform(i, aTime, data);
	}

	for(uint64_t i = 0; i < data.size(); i++) {
		aModel.mNodes[i].localMatrix = glm::translate(glm::mat4(1.0f), data[i].t) * glm::mat4_cast(data[i].r) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0));
	}
}

Animation::~Animation() noexcept {}

float Animation::lerp(float aLast, float aNext, float aCurrent) noexcept {
	if(aCurrent <= aLast) return 0.0; //should not happen
	if(aCurrent >= aNext) return 1.0;
	return ((float)aCurrent - (float)aLast)/((float)aNext - (float)aLast); //should be in range 0-1
}
uint64_t Animation::getIndex(const SamplerData& aSampler, const float aTime) noexcept {
	for(uint64_t i = 0; i < aSampler.time.size()-1; i++) {
		if(aTime < aSampler.time[i+1]) return i;
	}
	return aSampler.time.back();
}
glm::vec3 Animation::interpolatePosition(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::vec3 position = glm::vec3(glm::mix(aSampler.value[start], aSampler.value[end], weight));
	return position;
}
glm::quat Animation::interpolateRotation(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::quat qstart = glm::quat(aSampler.value[start].w, aSampler.value[start].x, aSampler.value[start].y, aSampler.value[start].z);
	glm::quat qend = glm::quat(aSampler.value[end].w, aSampler.value[end].x, aSampler.value[end].y, aSampler.value[end].z);
	glm::quat rotation = glm::normalize(glm::slerp(qstart, qend, weight));
	return rotation;
}
glm::vec3 Animation::interpolateScale(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::vec3 scale = glm::vec3(glm::mix(aSampler.value[start], aSampler.value[end], weight));
	return scale;
}

void Animation::getLocalSamplerTransform(const uint64_t aSamplerId, const float aTime, std::vector<TRSData>& aTRSData) noexcept {
	auto& sampler = this->mSamplers[aSamplerId];

	//enforce TRS

	switch(sampler.type) {
		case(fastgltf::AnimationPath::Translation):
			aTRSData[sampler.nodeIndex].t = interpolatePosition(sampler, aTime);
			break;
		case(fastgltf::AnimationPath::Rotation):
			aTRSData[sampler.nodeIndex].r = interpolateRotation(sampler, aTime);
			break;
		case(fastgltf::AnimationPath::Scale):
			aTRSData[sampler.nodeIndex].s = interpolateScale(sampler, aTime);
			break;
		default:
			std::cerr << LogLevel::ERROR << "Weight animation is not supported!\n" << LogLevel::RESET;
			break;
	}
}
