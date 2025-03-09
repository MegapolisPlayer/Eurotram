#version 450 core

//vertex shader for billboarding

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec2 iTexCoords;

flat out int pInstanceId;
out vec4 pDBCoords;
out vec2 pTexCoords;

layout(location = 34) uniform mat4 uCamera;
layout(std430, binding = 35) readonly buffer sDroplets {
	mat4 transforms[];
};
layout(location = 36) uniform mat4 uViewMatrix;

//billboarding!
//also f*ck std430 layout and vec3s
layout(std430, binding = 37) readonly buffer sCenters {
	vec4 centers[];
};
layout(location = 39) uniform vec3 uCameraUp;
layout(location = 40) uniform vec3 uCameraRight;
layout(location = 41) uniform vec3 uWeatherCenter;

void main() {
	pInstanceId = gl_InstanceID;
	pTexCoords = iTexCoords;
	pDBCoords = uViewMatrix * transforms[gl_InstanceID] * vec4(iPos, 1.0);

	//billboarding - only for result
	vec3 billboardCoords =
		(uWeatherCenter+centers[gl_InstanceID].xyz)
		+ uCameraRight * iPos.x
		+ uCameraUp * iPos.y
	;
	gl_Position = uCamera * vec4(billboardCoords, 1.0);
};
