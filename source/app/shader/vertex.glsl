#version 450 core

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormals;
layout(location = 2) in vec2 iTexCoord;
layout(location = 3) in vec4 iBoneIds;
layout(location = 4) in vec4 iBoneWeights;

layout(location = 10) uniform mat4 uCamera;
layout(location = 11) uniform mat4 uMatrixModel;
layout(location = 12) uniform mat3 uMatrixNormal;

layout(location = 13) uniform mat4 uMatrixDiright;
layout(location = 14) uniform mat4 uMatrixFlashlight;

out vec2 pTexCoord;
flat out vec3 pNormals;
out vec3 pFragmentPos;

out vec4 pFragmentDirectionalLightPos;
out vec4 pFragmentFlashlightLightPos;
out vec4 pFragmentLeftFrontLightPos;
out vec4 pFragmentRightFrontLightPos;

const int MAX_BONES_PER_VERTEX = 4;

layout(std430, binding = 40) readonly buffer sBoneMatrices {
	mat4 boneMatrices[];
};

void main() {
	pTexCoord = iTexCoord;
	pNormals = uMatrixNormal * iNormals;

	vec4 finalPosition = vec4(iPosition, 0.0);

	/*
	vec4 finalPosition = vec4(0.0);

	for(int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
		if(iBoneIds[i] < -0.01) { continue; }
		finalPosition += (boneMatrices[int(iBoneIds[i])] * vec4(iPosition, 1.0) * iBoneWeights[i]);
	}
	*/

	pFragmentPos = vec3(uMatrixModel * finalPosition);
	pFragmentDirectionalLightPos = uMatrixDiright * uMatrixModel * finalPosition;
	pFragmentFlashlightLightPos = uMatrixFlashlight * uMatrixModel * finalPosition;

	gl_Position = uCamera * vec4(pFragmentPos, 1.0);
};
