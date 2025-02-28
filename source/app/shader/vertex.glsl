#version 450 core

//normal vertex shader

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormals;
layout(location = 2) in vec2 iTexCoord;
layout(location = 3) in float iMaterialId;
layout(location = 4) in vec4 iBoneIds;
layout(location = 5) in vec4 iBoneWeights;

layout(location = 10) uniform mat4 uCamera;

//of just mesh
layout(location = 11) uniform mat4 uMatrixModel;
layout(location = 12) uniform mat3 uMatrixNormal;

layout(location = 13) uniform mat4 uMatrixDiright;
layout(location = 14) uniform mat4 uMatrixFlashlight;

out vec2 pTexCoord;
flat out float pMatId; //id of material in GMS
flat out vec3 pNormals;
out vec3 pFragmentPos;

out vec4 pFragmentDirectionalLightPos;
out vec4 pFragmentFlashlightLightPos;
out vec4 pFragmentLeftFrontLightPos;
out vec4 pFragmentRightFrontLightPos;

const int MAX_BONES_PER_VERTEX = 4;
const float MIN_NOT_IGNORE = -0.5;

layout(std430, binding = 40) readonly buffer sBoneMatrices {
	mat4 boneMatrices[];
};

layout(std430, binding = 35) readonly buffer sInstanceMatrices {
	mat4 instanceMatrices[];
};
layout(location = 36) uniform int uIsInstancedRendering;

void main() {
	pTexCoord = iTexCoord;
	pMatId = iMaterialId;

	mat4 boneTransform = mat4(1.0); //identity

	//TODO condition if -1
	boneTransform += (boneMatrices[int(iBoneIds[0])] * iBoneWeights[0]);
	boneTransform += (boneMatrices[int(iBoneIds[1])] * iBoneWeights[1]);
	boneTransform += (boneMatrices[int(iBoneIds[2])] * iBoneWeights[2]);
	boneTransform += (boneMatrices[int(iBoneIds[3])] * iBoneWeights[3]);

	mat4 instanceTransform = mat4(1.0);
	if(uIsInstancedRendering == 1) {
		instanceTransform = instanceMatrices[gl_InstanceID];
	}

	pNormals = normalize(uMatrixNormal * iNormals);
	pFragmentPos = vec3(uMatrixModel * instanceTransform * boneTransform * vec4(iPosition, 1.0));
	pFragmentDirectionalLightPos = uMatrixDiright * uMatrixModel * instanceTransform * boneTransform * vec4(iPosition, 1.0);
	pFragmentFlashlightLightPos = uMatrixFlashlight * uMatrixModel * instanceTransform * boneTransform * vec4(iPosition, 1.0);

	gl_Position = uCamera * vec4(pFragmentPos, 1.0);
};
