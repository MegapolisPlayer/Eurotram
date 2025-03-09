#version 450 core

//fragment shader for rendering weather particles

flat in int pInstanceId;
in vec4 pGlobalCoords;
in vec4 pDBCoords;
in vec2 pTexCoords;
out vec4 oColor;

layout(location = 30) uniform vec4 sColor;
layout(std430, binding = 31) buffer sMove {
	int moves[];
};
//tex id 0
layout(binding = 0) uniform sampler2D uDepthTexture;

void main() {
	vec3 coords = pDBCoords.xyz / pDBCoords.w;
	float currentDepth = coords.z;

	vec2 texelSize = vec2(1.0) / textureSize(uDepthTexture, 0); //divide max coord by amount of texels in texture (as vec2)
	float closest = texture(uDepthTexture, coords.xy).r;

	if(currentDepth >= closest) {
		moves[pInstanceId] = 1;
		discard;
	}
	else {
		oColor = vec4(currentDepth*2.0, sColor.yzw);
	}
};
