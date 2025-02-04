#include "Animation.hpp"

#include "ModelLoading.hpp" //so model class works

Animation::Animation() noexcept  {}
void Animation::setStateAtTime(Model& aModel, const float aTime) noexcept {
	//TODO
	//https://gamedev.stackexchange.com/questions/200973/understanding-matrix-transformations-for-skeletal-animation
	//https://gamedev.net/forums/topic/703803-gltf-skinning-matrix-calculation/
	//https://github.com/SaschaWillems/Vulkan/blob/master/examples/gltfskinning/README.md BEST TUTORIAL
	//https://github.khronos.org/glTF-Tutorials/gltfTutorial/gltfTutorial_020_Skins.html

	//update all node matrices (not in Animation class, in Model)

	//calculate the interpolation stuff and apply here
	//apply joint matrix (local in our terminology) from parent(s)

	//end of TODO

	//calculate matrices
	std::vector<TRSData> data;
	data.resize(aModel.mNodes.size());
	for(uint64_t i = 0; i < this->mSamplers.size(); i++) {
		this->getBoneTransformations(aModel, i, aTime, data);
	}

	//multiply
	for(uint64_t i = 0; i < data.size(); i++) {
		if(aModel.mNodes[i].boneOutputMatrixId == -1) continue;
		std::cout << "Animating " << aModel.mNodes[i].name << '\n';
		aModel.mNodes[i].localMatrix = glm::scale(glm::mat4(1.0f), data[i].s);
		aModel.mNodes[i].localMatrix *= glm::mat4_cast(data[i].r);
		aModel.mNodes[i].localMatrix = glm::translate(aModel.mNodes[i].localMatrix, data[i].t);
	}

	//set data of nodes (joints)
	for(GLTFNode& n : aModel.mNodes) {
		if(n.boneOutputMatrixId == -1) continue;
		aModel.mBoneMatrices[n.boneOutputMatrixId] = glm::inverse(n.transformMatrix) * n.localMatrix * n.transformMatrix;
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
	glm::quat rotation = glm::normalize(glm::slerp(glm::quat(aSampler.value[start]), glm::quat(aSampler.value[end]), weight));
	return rotation;
}
glm::vec3 Animation::interpolateScale(const SamplerData& aSampler, const float aTime) noexcept {
	uint64_t start = getIndex(aSampler, aTime);
	uint64_t end = start+1;
	float weight = lerp(aSampler.time[start], aSampler.time[end], aTime);
	glm::vec3 scale = glm::vec3(glm::mix(aSampler.value[start], aSampler.value[end], weight));
	return scale;
}

void Animation::getBoneTransformations(Model& aModel, const uint64_t aSamplerId, const float aTime, std::vector<TRSData>& aTRSData) noexcept {
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
