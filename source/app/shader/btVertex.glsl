#version 450 core

//vertex shader for trigger boxes

layout(location = 0) in vec3 iPos;

flat out int pInstanceId;

layout(location = 30) uniform mat4 uCamera;
layout(std430, binding = 10) readonly buffer sTransforms {
	mat4 transforms[];
};

void main() {
	pInstanceId = gl_InstanceID;
	gl_Position = uCamera * transforms[pInstanceId] * vec4(iPos, 1.0);
};
