#version 450 core

//OIT compositing fragment shader
//combine with screenVertex

layout (binding = 0) uniform sampler2DMS uAccumRT; //reads from FBOs COLOR_ATTACHMENT_0 and COLOR_ATTACHMENT_1
layout (binding = 1) uniform sampler2DMS uRevealRT;

in vec2 pTexCoords;
out vec4 oColor;

const float EPSILON = 0.00001;

//from LearnOpenGL
bool isApproximatelyEqual(float a, float b) {
	return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}
float max3(vec3 v) {
	return max(max(v.x, v.y), v.z);
}

void main() {
	ivec2 coords = ivec2(gl_FragCoord.xy);
	float revealData = texelFetch(uRevealRT, coords, 0).r;

	//save calculation time
	//if(isApproximatelyEqual(revealData, 1.0f)) { discard; }

	vec4 accumData = texelFetch(uAccumRT, coords, 0);

	//suppress high values
	if (isinf(max3(abs(accumData.rgb))))
		accumData.rgb = vec3(accumData.a);

	//floating point precision...
	vec3 averageColor = accumData.rgb / max(accumData.a, EPSILON);

	//GL_ONE_MINUS_SRC_ALPHA
	oColor = vec4(averageColor, 1.0 - revealData);
};
