#version 450 core

//OIT combinating fragment shader
//combine with screenVertex

layout (binding = 0) uniform sampler2D uAccumRT; //reads from FBOs COLOR_ATTACHMENT_0 and COLOR_ATTACHMENT_1
layout (binding = 1) uniform sampler2D uRevealRT;

in vec2 pTexCoords;
layout (location = 0) out vec4 oColor;

void main() {
	vec4 accumData = texelFetch(uAccumRT, ivec2(gl_FragCoord.xy), 0);
	float revealData = texelFetch(uRevealRT, ivec2(gl_FragCoord.xy), 0).r;

	//blend func with params GL_ONE_MINUS_SRC_ALPHA ; GL_SRC_ALPHA
	oColor = vec4(accumData.rgb / max(accumData.a, 1e-5), revealData);
};
