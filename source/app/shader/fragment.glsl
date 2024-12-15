#version 450 core

//Standard shader for all objects (except light sources)

//NEVER EVER USE VEC3 IN SSBO-UBO LAYOUT!
//STD140 AND STD430 LAYOUT PACKING IS IDIOTIC AND ALIGNS TO VEC4
//https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o
struct Material {
	vec4 color;
	vec4 specular;
	float ior;
	float shininess;

	float textureAmount;
	int textureSlot;
	float textureOpacity;
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
	layout(align = 16) Material mat1;
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

float specularFromIOR(float aIOR) {
	return clamp((((aIOR - 1)/(aIOR + 1))*((aIOR - 1)/(aIOR + 1)))*12.5, 0.0, 1.0);
}

float diffuseComp(vec3 aNormalizedNormal, vec3 aLightDirection) {
	return max(dot(aNormalizedNormal, aLightDirection), 0.0); //dot product - "how" much is vector in our aLightDirection
}
float specularComp(vec3 aNormalizedNormal, vec3 aLightDirection, vec3 aViewDirection, float aShininess) {
	vec3 halfwayDir = normalize(aLightDirection + aViewDirection); //halfway direction - Blinn-Phong
	return pow(max(dot(aNormalizedNormal, halfwayDir), 0.0), aShininess);
}

//calculate specular only if some diffuse present

vec3 calculateDirectional(vec3 aColor, vec3 aNormalizedNormal, vec3 aViewDirection, Material aMat, Dirlight aLight) {
	vec3 lightDirection = normalize(-aLight.direction.xyz); //vector from (NOT TO) the light
	float diffuseValue = diffuseComp(aNormalizedNormal, lightDirection);
	float specularValue = specularFromIOR(aMat.ior) * specularComp(aNormalizedNormal, lightDirection, aViewDirection, aMat.shininess) * float(diffuseValue >= 0.01);

	float override = float(aMat.brightness >= diffuseValue + specularValue + uAmbientLight);
	return mix(
		((vec3(diffuseValue) * aColor + vec3(specularValue) * aMat.specular.xyz)) * aLight.color.xyz,
		vec3(aMat.brightness),
		vec3(override)
	);
}

vec3 calculatePoint(vec3 aColor, vec3 aNormalizedNormal, vec3 aViewDirection, vec3 aFragPos, Material aMat, Pointlight aLight) {
	float distanceFromLight = length(aLight.position.xyz - aFragPos);
	vec3 lightDirection = normalize(aLight.position.xyz - aFragPos); //subtracting vectors

	float diffuseValue = diffuseComp(aNormalizedNormal, lightDirection);
	float specularValue = specularFromIOR(aMat.ior) * specularComp(aNormalizedNormal, lightDirection, aViewDirection, aMat.shininess) * float(diffuseValue >= 0.01);

	float override = float(aMat.brightness >= diffuseValue + specularValue + uAmbientLight);
	return mix(
		((vec3(diffuseValue) * aColor + vec3(specularValue) * aMat.specular.xyz)) * aLight.color.xyz,
			   vec3(aMat.brightness),
			   vec3(override)
	);
}

vec3 calculateSpot(vec3 aColor, vec3 aNormalizedNormal, vec3 aViewDirection, vec3 aFragPos, Material aMat, Spotlight aLight) {
	float distanceFromLight = length(aLight.position.xyz - aFragPos);
	vec3 lightDirection = normalize(aLight.position.xyz - aFragPos);

	//dot product returns cosine of angle
	float theta = acos(dot(lightDirection, normalize(-aLight.direction.xyz)));
	float epsilon = -5.0; //extra cutoff angle

	//divide difference between outer angle and our angle by outer angle
	float strength = clamp((theta - (aLight.radius + 5.0)) / -5.0, 0.0, 1.0);

	float diffuseValue = diffuseComp(aNormalizedNormal, lightDirection);
	float specularValue = specularFromIOR(aMat.ior) * specularComp(aNormalizedNormal, lightDirection, aViewDirection, aMat.shininess) * float(diffuseValue >= 0.01);

	float override = float(aMat.brightness >= diffuseValue + specularValue + uAmbientLight);
	return mix(
		((vec3(diffuseValue) * aColor + vec3(specularValue) * aMat.specular.xyz)) * aLight.color.xyz,
			   vec3(aMat.brightness),
			   vec3(override)
	);
}

void main() {
	vec4 baseColor = mix(mat1.color, texture(uTextures[mat1.textureSlot], pTexCoord), mat1.textureAmount);

	vec3 normalizedNormal = normalize(pNormals);
	vec3 viewDirection = normalize(uCameraPosition - pFragmentPos);

	//calc dl - directional light
	vec3 lighting = calculateDirectional(baseColor.xyz, normalizedNormal, viewDirection, mat1, dl);

	//TODO lighting too bright - issue in point and spot lights
	//checked ambient light OK
	//checked mix line OK
	//problem in both point and spot - when alone also issue
	//checked amount of lights OK
	//specularComp OK
	//diffuseComp OK
	//directional not an issue
	//multiplying diffuse OR specular doesnt do anything!

	//when ambient 0 = lights up as normal
	//when 1 = full white!!!!

	//calculate point lights
	for(int i = 0; i < pointlights.length(); i++) {
		lighting += calculatePoint(
			baseColor.xyz,
			normalizedNormal, viewDirection, pFragmentPos,
			mat1, pointlights[i]
		);
	}

	//calculate spot lights
	for(int i = 0; i < spotlights.length(); i++) {
		lighting += calculateSpot(
			baseColor.xyz,
			normalizedNormal, viewDirection, pFragmentPos,
			mat1, spotlights[i]
		);
	}

	//lighting += vec3(uAmbientLight);

	//putting together
	oColor = vec4(lighting, mat1.textureOpacity); //normal calc
};
