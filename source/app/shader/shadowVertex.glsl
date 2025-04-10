#version 450 core

//vertex shader for rendering to depthmap

layout(location = 0) in vec3 iPosition;
layout(location = 4) in vec4 iBoneIds;
layout(location = 5) in vec4 iBoneWeights;

layout(location = 90) uniform mat4 lightProjection;
layout(location = 91) uniform mat4 model;

layout(std430, binding = 40) readonly buffer sBoneMatrices {
	mat4 boneMatrices[];
};
layout(std430, binding = 35) readonly buffer sInstanceMatrices {
	mat4 instanceMatrices[];
};
layout(location = 36) uniform int uIsInstancedRendering;

const int MAX_BONES_PER_VERTEX = 4;
const float MIN_NOT_IGNORE = -0.5;

void main() {
	mat4 boneTransform = mat4(1.0); //identity

	boneTransform += (boneMatrices[int(iBoneIds[0])] * max(0.0, iBoneWeights[0]));
	boneTransform += (boneMatrices[int(iBoneIds[1])] * max(0.0, iBoneWeights[1]));
	boneTransform += (boneMatrices[int(iBoneIds[2])] * max(0.0, iBoneWeights[2]));
	boneTransform += (boneMatrices[int(iBoneIds[3])] * max(0.0, iBoneWeights[3]));

	mat4 instanceTransform = mat4(1.0);
	if(uIsInstancedRendering == 1) {
		instanceTransform = instanceMatrices[gl_InstanceID];
	}

	//removed bone transform
	gl_Position = lightProjection * model * instanceTransform * vec4(iPosition, 1.0);
};
