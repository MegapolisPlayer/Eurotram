#version 450 core

//fragment shader for trigger boxes

layout(location = 0) out vec4 oColor;
layout(location = 1) out float oReveal; //for OIT

layout(std430, binding = 11) readonly buffer sColors {
	vec4 colors[];
};

flat in int pInstanceId;

void main() {
	vec4 baseColor = colors[pInstanceId];

	float weight = clamp(pow(min(1.0, baseColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

	oColor = vec4(baseColor.rgb * baseColor.a, baseColor.a) * weight;
	oReveal = baseColor.a;
};
