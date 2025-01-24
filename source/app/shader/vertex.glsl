#version 450 core

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormals;
layout(location = 2) in vec2 iTexCoord;

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

void main() {
	pTexCoord = iTexCoord;
	pNormals = uMatrixNormal * iNormals;
	pFragmentPos = vec3(uMatrixModel * vec4(iPosition, 1.0));

	pFragmentDirectionalLightPos = uMatrixDiright * uMatrixModel * vec4(iPosition, 1.0);
	pFragmentFlashlightLightPos = uMatrixFlashlight * uMatrixModel * vec4(iPosition, 1.0);

	gl_Position = uCamera * vec4(pFragmentPos, 1.0);
};
