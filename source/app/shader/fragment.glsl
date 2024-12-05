#version 450 core

//Standard shader for all objects (except light sources)

out vec4 oColor;

in vec2 pTexCoord;
in vec3 pNormals;
in vec3 pFragmentPos;

struct Material {
	float diffuse;
	float specular;
	float roughness;
	float shininess;

	float textureAmount;
	int textureSlot;
	float transparency;
	float brightness;
	vec3 color;
};

layout(std430, binding = 0) readonly buffer sMaterial {
	layout(align = 16) Material mat1;
};

layout(location = 100) uniform vec3 uLightPos;
layout(location = 101) uniform vec3 uLightColor;
layout(location = 102) uniform float uAmbientLight;

layout(location = 110) uniform vec3 uCameraPosition;

layout(location = 200) uniform sampler2D uTextures[32];

void main() {
	vec4 baseColor = mix(vec4(mat1.color, mat1.transparency), texture(uTextures[mat1.textureSlot], pTexCoord), vec4(mat1.textureAmount));

	//lighting component
	vec3 normalizedNormal = normalize(pNormals);
	vec3 lightPosDirection = normalize(uLightPos - pFragmentPos); //subtracting vectors

	//diffuse
	float diffuseValue = max(dot(normalizedNormal, lightPosDirection), 0.0); //dot product - "how" much is vector in our lightPosDirection

	//specular
	vec3 viewDirection = normalize(uCameraPosition - pFragmentPos);
	vec3 halfwayDir = normalize(lightPosDirection + viewDirection); //halfway direction - Blinn-Phong
	float specularValue = mat1.specular * pow(max(dot(normalizedNormal, halfwayDir), 0.0), mat1.shininess*128.0);

	//old Phong
	//vec3 reflectionDirection = reflect(-lightPosDirection, normalizedNormal);
	//float specularValue = mat1.specular * pow(max(dot(viewDirection, reflectionDirection), 0.0), mat1.shininess*128.0);

	//weight lighting
	float lightingValue = uAmbientLight + diffuseValue*0.5 + specularValue;
	float override = float(mat1.brightness >= lightingValue);

	vec3 lighting = mix(lightingValue * uLightColor, vec3(mat1.brightness), vec3(override));

	//putting together
	oColor = baseColor * vec4(lighting, 1.0);
};
