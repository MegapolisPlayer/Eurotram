#version 450 core

//vertex shader for billboarding TODO

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoords;

flat out int pInstanceId;
out vec4 pGlobalCoords;
out vec4 pDBCoords;
out vec2 pTexCoords;

layout(location = 34) uniform mat4 uCamera;
layout(std430, binding = 35) readonly buffer sDroplets {
	mat4 transforms[];
};
layout(location = 36) uniform mat4 uViewMatrix;

void main() {
	pInstanceId = gl_InstanceID;
	pTexCoords = iTexCoords;
	pDBCoords = uViewMatrix * transforms[gl_InstanceID] * vec4(iPos.x, 0.0, iPos.y, 1.0);
	pGlobalCoords = transforms[gl_InstanceID] * vec4(iPos.x, 0.0, iPos.y, 1.0);
	gl_Position = uCamera * pGlobalCoords;
};
