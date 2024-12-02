#version 450 core

out vec4 oColor;

in vec2 pTexCoord;
in vec3 pNormals;
in vec3 pFragmentPos;

layout(location = 100) uniform sampler2D uTexture;

layout(location = 101) uniform vec3 uObjectColor;
layout(location = 102) uniform float uIgnoreTexture;

layout(location = 103) uniform vec3 uLightPos;
layout(location = 104) uniform vec3 uLightColor;

layout(location = 105) uniform vec3 uCameraPosition;

layout(location = 110) uniform float uAmbientStrength;
layout(location = 111) uniform float uSpecularStrength;

void main() {
	vec4 baseColor = vec4(
		vec4(1-uIgnoreTexture) * texture(uTexture, pTexCoord) +
		vec4(uIgnoreTexture) * vec4(uObjectColor, 1.0)
			);

	//lighting component
	vec3 normalizedNormal = normalize(pNormals);
	vec3 lightPosDirection = normalize(uLightPos - pFragmentPos); //subtracting vectors

	//diffuse
	float diffuseValue = max(dot(normalizedNormal, lightPosDirection), 0.0); //dot product - "how" much is vector in our lightPosDirection

	//specular
	vec3 viewDirection = normalize(uCameraPosition - pFragmentPos);
	vec3 reflectionDirection = reflect(-lightPosDirection, normalizedNormal);
	float specularValue = uSpecularStrength * pow(max(dot(viewDirection, reflectionDirection), 0.0), 32); //32 = shininess

	vec3 lighting = vec3(uAmbientStrength + diffuseValue + specularValue) * uLightColor;

	//putting together
	oColor = baseColor * vec4(lighting, 1.0);
};
