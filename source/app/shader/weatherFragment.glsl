#version 450 core

//fragment shader for rendering weather particles

flat in int pInstanceId;
in vec4 pDBCoords;
in vec2 pTexCoords;

layout(location = 0) out vec4 oColor;
layout(location = 1) out float oReveal; //for OIT

layout(location = 30) uniform vec4 sColor;
//tex id 0
layout(binding = 0) uniform sampler2D uDepthTexture;

void main() {
	vec3 coords = pDBCoords.xyz / pDBCoords.w;
	coords = (coords * 0.5) + 0.5; //convert to 0...1 from -1...1
	float currentDepth = coords.z;

	vec2 texelSize = vec2(1.0) / textureSize(uDepthTexture, 0); //divide max coord by amount of texels in texture (as vec2)
	float closest = texture(uDepthTexture, coords.xy).r;

	if(currentDepth >= closest) {
		discard;
	}
	else {
		//some random weight function
		//just gets how much color
		float weight = clamp(pow(min(1.0, sColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

		oColor = vec4(sColor.rgb * sColor.a, sColor.a) * weight;
		oReveal = sColor.a;
	}
};
