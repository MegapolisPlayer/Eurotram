#version 450 core

layout(location = 0) in vec3 iPosition;

layout(location = 71) uniform mat4 uLightingShadowMatrix;
layout(location = 70) uniform mat4 uModel;

void main() {
	gl_Position = uLightingShadowMatrix * uModel * vec4(iPosition, 1.0);
};
