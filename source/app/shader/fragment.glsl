#version 450 core

//Standard shader for all objects (except light sources)

//NEVER EVER USE VEC3 IN SSBO-UBO LAYOUT!
//STD140 AND STD430 LAYOUT PACKING IS IDIOTIC AND ALIGNS TO VEC4
//https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o
struct Material {
	vec4 color;
	vec4 diffuse;
	vec4 specular;
	float shininess;

	float textureAmount;
	int textureSlot;
	float brightness;
};

struct Dirlight {
	vec4 direction;
	vec4 color;
};
struct Pointlight {
	vec4 position;
	vec4 color;

	float constant;
	float linear;
	float quadratic;
};
struct Spotlight {
	vec4 position;
	vec4 direction;
	vec4 color;
	float radius;

	float constant;
	float linear;
	float quadratic;
};

out vec4 oColor;
in vec2 pTexCoord;
in vec3 pNormals;
in vec3 pFragmentPos;

layout(std430, binding = 50) readonly buffer sMaterial {
	Material mat1;
};
layout(std430, binding = 51) readonly buffer sDirlight {
	Dirlight dl;
};
layout(std430, binding = 52) readonly buffer sPointlights {
	Pointlight pointlights[];
};
layout(std430, binding = 53) readonly buffer sSpotlights {
	Spotlight spotlights[];
};

layout(location = 200) uniform float uAmbientLight;
layout(location = 210) uniform vec3 uCameraPosition;
layout(location = 232) uniform sampler2D uTextures[32];

float attenuation(float aDistance, float aConstant, float aLinear, float aQuadratic) {
	return 1.0/(aConstant + aLinear*aDistance + aQuadratic*aDistance*aDistance);
}

float diffuseComp(vec3 aNormalizedNormal, vec3 aLightDirection) {
	return max(dot(aNormalizedNormal, aLightDirection), 0.0); //dot product - "how" much is vector in our aLightDirection
}
float specularComp(vec3 aNormalizedNormal, vec3 aLightDirection, vec3 aViewDirection, float aShininess) {
	vec3 halfwayDir = normalize(aLightDirection + aViewDirection); //halfway direction - Blinn-Phong
	return pow(max(dot(aNormalizedNormal, halfwayDir), 0.0), aShininess*128.0);
}

//calculate specular only if some diffuse present

vec3 calculateDirectional(vec3 aNormalizedNormal, vec3 aViewDirection, Material aMat, Dirlight aLight) {
	vec3 lightDirection = normalize(-aLight.direction.xyz); //vector from (NOT TO) the light
	float diffuseValue = 0.5 * diffuseComp(aNormalizedNormal, lightDirection);
	float specularValue = specularComp(aNormalizedNormal, lightDirection, aViewDirection, aMat.shininess) * float(diffuseValue >= 0.01);
	float lightingValue = diffuseValue + specularValue;

	float override = float(aMat.brightness >= lightingValue);
	return mix((diffuseValue * aMat.diffuse.xyz + specularValue * aMat.specular.xyz) * aLight.color.xyz, vec3(aMat.brightness), vec3(override));
}

vec3 calculatePoint(vec3 aNormalizedNormal, vec3 aViewDirection, vec3 aFragPos, Material aMat, Pointlight aLight) {
	float distanceFromLight = length(aLight.position.xyz - aFragPos);
	vec3 lightPosDirection = normalize(aLight.position.xyz - aFragPos); //subtracting vectors

	float diffuseValue = 0.5 * diffuseComp(aNormalizedNormal, lightPosDirection);
	float specularValue = specularComp(aNormalizedNormal, lightPosDirection, aViewDirection, aMat.shininess) * float(diffuseValue >= 0.01);
	float lightingValue = (diffuseValue + specularValue) *
	attenuation(distanceFromLight, aLight.constant, aLight.linear, aLight.quadratic);

	float override = float(aMat.brightness >= lightingValue);
	return mix((diffuseValue * aMat.diffuse.xyz + specularValue * aMat.specular.xyz) * aLight.color.xyz, vec3(aMat.brightness), vec3(override));
}

vec3 calculateSpot(vec3 aNormalizedNormal, vec3 aViewDirection, vec3 aFragPos, Material aMat, Spotlight aLight) {
	float distanceFromLight = length(aLight.position.xyz - aFragPos);
	vec3 lightPosDirection = normalize(aLight.position.xyz - aFragPos);

	//dot product returns cosine of angle
	float theta = acos(dot(lightPosDirection, normalize(-aLight.direction.xyz)));
	float epsilon = -5.0; //extra cutoff angle

	//divide difference between outer angle and our angle by outer angle
	float strength = clamp((theta - (aLight.radius + 5.0)) / -5.0, 0.0, 1.0);

	float diffuseValue = 0.5 * diffuseComp(aNormalizedNormal, lightPosDirection);
	float specularValue = specularComp(aNormalizedNormal, lightPosDirection, aViewDirection, aMat.shininess) * float(diffuseValue >= 0.01);
	float lightingValue = (diffuseValue + specularValue) *
	attenuation(distanceFromLight, aLight.constant, aLight.linear, aLight.quadratic) * strength;

	float override = float(aMat.brightness >= lightingValue);
	return mix((diffuseValue * aMat.diffuse.xyz + specularValue * aMat.specular.xyz) * aLight.color.xyz, vec3(aMat.brightness), vec3(override));
}

void main() {
	vec4 baseColor = mix(mat1.color, texture(uTextures[mat1.textureSlot], pTexCoord), vec4(mat1.textureAmount));

	vec3 normalizedNormal = normalize(pNormals);
	vec3 viewDirection = normalize(uCameraPosition - pFragmentPos);

	//calc dl - directional light
	vec3 lighting = calculateDirectional(normalizedNormal, viewDirection, mat1, dl);

	//calculate point lights
	for(int i = 0; i < pointlights.length(); i++) {
		lighting += calculatePoint(
			normalizedNormal, viewDirection, pFragmentPos,
			mat1, pointlights[i]
		);
	}

	//calculate spot lights
	for(int i = 0; i < spotlights.length(); i++) {
		lighting += calculateSpot(
			normalizedNormal, viewDirection, pFragmentPos,
			mat1, spotlights[i]
		);
	}

	//putting together
	oColor = baseColor * vec4(max(lighting, vec3(uAmbientLight)), 1.0);
	//oColor = vec4(vec3(float(pointlights.length())*0.5), 1.0); //debug line
};
