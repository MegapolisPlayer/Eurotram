#version 450 core

//Standard shader for all objects (except shadows)

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
	float inner;
	float outer;

	float constant;
	float linear;
	float quadratic;
};

out vec4 oColor;
in vec2 pTexCoord;
in vec3 pNormals;
in vec3 pFragmentPos;
in vec4 pLightFragmentPos;

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

//reflectivity
float specularFromIOR(float aIOR) {
	return clamp((((aIOR - 1)/(aIOR + 1))*((aIOR - 1)/(aIOR + 1)))*12.5, 0.0, 1.0);
}

float diffuseComp(vec3 aNormalizedNormal, vec3 aLightDirection) {
	return max(dot(aNormalizedNormal, aLightDirection), 0.0); //dot product - "how" much is vector in our aLightDirection
}
//shininess
float specularComp(vec3 aNormalizedNormal, vec3 aLightDirection, vec3 aViewDirection, float aShininess) {
	vec3 halfwayDir = normalize(aLightDirection + aViewDirection); //halfway direction - Blinn-Phong
	return pow(max(dot(aNormalizedNormal, halfwayDir), 0.0), aShininess);
}

//calculate specular only if some diffuse present

vec3 getSpecular(
	vec3 aLightColor, float aDst, float aAC, float aAL, float aAQ,
	vec3 aNormalizedNormal, vec3 aLightDirection, vec3 aViewDirection,
	Material aMat
) {
	return aLightColor * specularFromIOR(aMat.ior) * specularComp(aNormalizedNormal, aLightDirection, aViewDirection, aMat.shininess)* attenuation(aDst, aAC, aAL, aAQ);
}

vec3 calculateDirectional(vec3 aColor, vec3 aNormalizedNormal, vec3 aViewDirection, Material aMat, Dirlight aLight) {
	return aLight.color.xyz * diffuseComp(aNormalizedNormal, aLight.direction.xyz);
}

vec3 calculatePoint(float aDst, vec3 aNormalizedNormal, vec3 aLightDirection, vec3 aFragPos, Material aMat, Pointlight aLight) {
	return aLight.color.xyz * attenuation(aDst, aLight.constant, aLight.linear, aLight.quadratic) * diffuseComp(aNormalizedNormal, aLightDirection);
}

float calculateSpotStrength(vec3 aLightDirection, Spotlight aLight) {
	//dot product returns cosine of angle
	float theta = dot(aLightDirection, normalize(-aLight.direction.xyz));
	//divide by cosine difference epsilon
	return clamp((theta - aLight.outer) / (aLight.inner - aLight.outer), 0.0, 1.0);
}

vec3 calculateSpot(float aDst, vec3 aNormalizedNormal, vec3 aLightDirection, vec3 aFragPos, Material aMat, Spotlight aLight) {
	return aLight.color.xyz * attenuation(aDst, aLight.constant, aLight.linear, aLight.quadratic) * diffuseComp(aNormalizedNormal, aLightDirection);
}

float calculateShadow(vec3 aNormalizedNormal, vec3 aLightDirection) {
	//copy some internal openGL step (convert to normalized device coords)
	vec3 projectedCoords = pLightFragmentPos.xyz / pLightFragmentPos.w; //range -1 to +1
	//convert to 0, 1
	projectedCoords = (projectedCoords * 0.5) + 0.5;

	float bias = max(0.05 * (1.0 - dot(aNormalizedNormal, aLightDirection)), 0.005);
	//our depth in NDC > depth in map
	return 1.0-float(projectedCoords.z > texture(uTextures[1], projectedCoords.xy).r);
}

void main() {
	vec4 baseColor = mix(mat1.color, texture(uTextures[mat1.textureSlot], pTexCoord), mat1.textureAmount);

	vec3 normalizedNormal = normalize(pNormals);
	vec3 viewDirection = normalize(uCameraPosition - pFragmentPos);

	vec3 lighting = calculateDirectional(dl.color.xyz, normalizedNormal, viewDirection, mat1, dl);
	vec3 directionalLightDirection = normalize(-dl.direction.xyz); //vector from (NOT TO) the light
	vec3 lightingSpecular = getSpecular(dl.color.xyz, 0.0, 1.0, 0.0, 0.0, normalizedNormal, directionalLightDirection, viewDirection, mat1);

	//putting together
	for(int i = 0; i < pointlights.length(); i++) {
		vec3 lightDirection = normalize(pointlights[i].position.xyz - pFragmentPos);
		float dst = length(pointlights[i].position.xyz - pFragmentPos);

		lighting += calculatePoint(dst, normalizedNormal, lightDirection, viewDirection, mat1, pointlights[i]);
		lightingSpecular += getSpecular(
			pointlights[i].color.xyz, dst,
			pointlights[i].constant, pointlights[i].linear, pointlights[i].quadratic,
			normalizedNormal, lightDirection, viewDirection, mat1
		);
	}
	for(int i = 0; i < spotlights.length(); i++) {
		vec3 lightDirection = normalize(spotlights[i].position.xyz - pFragmentPos);
		float dst = length(spotlights[i].position.xyz - pFragmentPos);
		float strength = calculateSpotStrength(lightDirection, spotlights[i]);

		lighting += calculateSpot(dst, normalizedNormal, lightDirection, viewDirection, mat1, spotlights[i]) * strength;
		lightingSpecular += getSpecular(
			spotlights[i].color.xyz, dst,
			spotlights[i].constant, spotlights[i].linear, spotlights[i].quadratic,
			normalizedNormal, lightDirection, viewDirection, mat1
		) * strength;
	}

	oColor = vec4(
		baseColor.xyz * clamp(uAmbientLight + (lighting + lightingSpecular) * calculateShadow(normalizedNormal, directionalLightDirection), 0.0, 1.0),
		mat1.textureOpacity); //normal calc
};
