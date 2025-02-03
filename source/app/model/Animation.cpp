#include "Animation.hpp"

#include "ModelLoading.hpp" //so model class works

Animation::Animation() noexcept  {}
void Animation::setStateAtTime(Model& aModel, const float aTime) noexcept {
	//TODO rewrite - animate JOINTS not all nodes!!!

	//calculate matrices
	std::vector<TRSData> data;
	data.resize(aModel.mNodes.size());
	for(uint64_t i = 0; i < this->mSamplers.size(); i++) {
		this->getBoneTransformations(aModel, i, aTime, data);
	}

	//multiply
	for(uint64_t i = 0; i < data.size(); i++) {
		aModel.mNodes[i].localMatrix = data[i].s * data[i].r * data[i].t;
	}

	//set data of nodes (joints)
	for(GLTFNode& n : aModel.mNodes) {
		if(n.idOfSkin == -1) continue;
		aModel.mBoneMatrices[n.boneOutputMatrixId] = glm::inverse(n.transformMatrix) * n.localMatrix * n.inverseBindMatrix;
		std::cout << n.name << '@' << aTime << ": " << 	aModel.mBoneMatrices[n.boneOutputMatrixId] * glm::vec4(1.0) << '\n';
	}
}

Animation::~Animation() noexcept {}

float Animation::lerp(float aLast, float aNext, float aCurrent) noexcept {
	if(aCurrent < aLast || aCurrent > aNext) return 0.0; //should not happen
	return ((float)aCurrent - (float)aLast)/((float)aNext - (float)aLast); //should be in range 0-1
}
uint64_t Animation::getIndex(const SamplerData& aSampler, const float aTime) noexcept {
	for(uint64_t i = 0; i < aSampler.time.size()-1; i++) {
		if(aTime < aSampler.time[i+1]) return i;
	}
	return UINT64_MAX;
}
glm::mat4 Animation::interpolatePosition(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::vec3 position = glm::vec3(glm::mix(aSampler.value[start], aSampler.value[end], weight));
	return glm::translate(glm::mat4(1.0), position);
}
glm::mat4 Animation::interpolateRotation(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::quat rotation = glm::normalize(glm::slerp(glm::quat(aSampler.value[start]), glm::quat(aSampler.value[end]), weight));
	return glm::mat4_cast(rotation);
}
glm::mat4 Animation::interpolateScale(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::vec3 scale = glm::vec3(glm::mix(aSampler.value[start], aSampler.value[end], weight));
	return glm::scale(glm::mat4(1.0), scale);
}

void Animation::getBoneTransformations(Model& aModel, const uint64_t aSamplerId, const float aTime, std::vector<TRSData>& aTRSData) noexcept {
	auto& sampler = this->mSamplers[aSamplerId];

	//enforce TRS

	switch(sampler.type) {
		case(fastgltf::AnimationPath::Translation):
			aTRSData[sampler.nodeIndex].t = interpolatePosition(sampler, aTime);
			std::cout << aModel.mNodes[sampler.nodeIndex].name << " T " << glm::vec4(1.0f) * aTRSData[sampler.nodeIndex].t << '\n';
			break;
		case(fastgltf::AnimationPath::Rotation):
			aTRSData[sampler.nodeIndex].r = interpolateRotation(sampler, aTime);
			std::cout << aModel.mNodes[sampler.nodeIndex].name << " R " << glm::vec4(1.0f) * aTRSData[sampler.nodeIndex].r << '\n';
			break;
		case(fastgltf::AnimationPath::Scale):
			aTRSData[sampler.nodeIndex].s = interpolateScale(sampler, aTime);
			std::cout << aModel.mNodes[sampler.nodeIndex].name << " S " << glm::vec4(1.0f) * aTRSData[sampler.nodeIndex].s << '\n';
			break;
		default:
			std::cerr << LogLevel::ERROR << "Weight animation is not supported!\n" << LogLevel::RESET;
			break;
	}
}
