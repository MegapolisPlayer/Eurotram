#version 450 core

layout(location = 0) in vec3 iPosition;

layout(location = 90) uniform mat4 lightProjection;
layout(location = 91) uniform mat4 model;

void main() {
	gl_Position = lightProjection * model * vec4(iPosition, 1.0);
};
