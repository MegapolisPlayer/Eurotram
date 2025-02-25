#version 450 core

//normal fragment shader for all objects

layout(location = 0) out vec4 oColor;
layout(location = 1) out float oReveal; //for OIT

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

	//16 byte aligned
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

	int lightType;
};

in vec2 pTexCoord;
flat in float pMatId;
in vec3 pNormals;
in vec3 pFragmentPos;

//for shadows
in vec4 pFragmentDirectionalLightPos;
in vec4 pFragmentFlashlightLightPos;
in vec4 pFragmentLeftFrontLightPos;
in vec4 pFragmentRightFrontLightPos;

layout(std430, binding = 50) readonly buffer sMaterial {
	Material materials[];
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
layout(location = 232) uniform sampler2D uTextures[32]; //realistically hardware with OpenGL 4.5/4.6 which we need anyway supports this, we test at startup

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
	return pow((dot(aNormalizedNormal, halfwayDir) + 1.0) / 2.0, aShininess);
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

const float BIAS_COEF = 0.005;
const float SPECULAR_SHADOW_CUTOFF = 0.01;

float calculateShadows(int aTextureId, vec4 aCoords, vec3 aNormalizedNormal, vec3 aLightDirection) {
	float shadow = 0.0;
	vec3 coords = aCoords.xyz / aCoords.w;
	//only if within frustum
	if(coords.z <= 1.0) {
		coords = (coords * 0.5) + 0.5; //convert from OpenGL render coords to UV coords

		//float closest = texture(uTextures[1], coords.xy).r; //single-dimensional texture
		float current = coords.z;
		float bias = max(BIAS_COEF * (dot(aNormalizedNormal, aLightDirection)), BIAS_COEF/100.0);
		//shadow = float(current - bias > closest);

		vec2 texelSize = vec2(1.0) / textureSize(uTextures[aTextureId], 0); //divide max coord by amount of texels in texture (as vec2)

		//PCF - percentage-closer filtering - adding softer shadows
		//this is a kind of "fake" PCF implementation - we dont have an explicit boolean field, but we are based on a condition
		//we use a 5x5 kernel

		//average from neighboring texels
		for(int x = -2; x <= 1; x++) {
			for(int y = -2; y <= 2; y++) {
				float closest = texture(uTextures[aTextureId], coords.xy + (vec2(x, y) * texelSize)).r; //single-dimensional texture

				shadow += float(current - bias > closest);
			}
		}

		shadow /= 25.0; //average it, clamp to 0 - 1
	}

	return 1.0 - shadow;
}

//31 - directional shadows fbo
const int DIRECTIONAL_TEXBIND = 31;
//tram front lights FBOS
const int TRAM_FRONT_LEFT_TEXBIND = 30;
const int TRAM_FRONT_RIGHT_TEXBIND = 29;
const int FLASHLIGHT_TEXBIND = 28;

layout(location = 37) uniform int uWriteToRenderTargets; //0 - nothing, 1 - discard transparent, 2 - discard solid
layout(location = 38) uniform int uIgnoreLighting; //if 1 - ignore lighting

void main() {
	Material mat1 = materials[int(round(pMatId))];

	vec4 baseColor = mix(mat1.color, texture(uTextures[mat1.textureSlot], pTexCoord), mat1.textureAmount);

	//no other way - i dont see how to integrate stencil here
	if(
		(baseColor.w <= 0.95 && uWriteToRenderTargets == 1) ||
		(baseColor.w >= 0.95 && uWriteToRenderTargets == 2)
	) {
		discard;
	}

	//this branching ok - only changes depending on draw call
	//also saves a bunch of computation
	vec3 lightingMultiplier;
	if(uIgnoreLighting == 0) {
		vec3 normalizedNormal = normalize(pNormals);
		vec3 viewDirection = normalize(uCameraPosition - pFragmentPos);

		vec3 directionalLightDirection = normalize(-dl.direction.xyz); //vector from (NOT TO) the light
		float directionShadow = calculateShadows(DIRECTIONAL_TEXBIND, pFragmentDirectionalLightPos, normalizedNormal, directionalLightDirection);
		vec3 lighting = calculateDirectional(dl.color.xyz, normalizedNormal, viewDirection, mat1, dl) * directionShadow;
		vec3 lightingSpecular = getSpecular(dl.color.xyz, 0.0, 1.0, 0.0, 0.0, normalizedNormal, directionalLightDirection, viewDirection, mat1) * float(directionShadow >= SPECULAR_SHADOW_CUTOFF);

		//putting together
		for(int i = 0; i < pointlights.length(); i++) {
			vec3 lightDirection = normalize(pointlights[i].position.xyz - pFragmentPos);
			float dst = length(pointlights[i].position.xyz - pFragmentPos);

			//pointlights have no shadows

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

			//spotlight shadows
			//calculate normal lighting values, then multiply by shadow value if applicable

			float shadow = 1.0;

			switch(spotlights[i].lightType) {
				case(1):
					shadow = calculateShadows(FLASHLIGHT_TEXBIND, pFragmentFlashlightLightPos, normalizedNormal, lightDirection);
					break;
				case(2):
					shadow = calculateShadows(TRAM_FRONT_LEFT_TEXBIND, pFragmentLeftFrontLightPos, normalizedNormal, lightDirection);
					break;
				case(3):
					shadow = calculateShadows(TRAM_FRONT_RIGHT_TEXBIND, pFragmentRightFrontLightPos, normalizedNormal, lightDirection);
					break;
			}

			lighting += calculateSpot(
				dst, normalizedNormal, lightDirection, viewDirection, mat1, spotlights[i]
			) * strength * shadow;
			//specular present only if no shadow
			lightingSpecular += getSpecular(
				spotlights[i].color.xyz, dst,
				spotlights[i].constant, spotlights[i].linear, spotlights[i].quadratic,
				normalizedNormal, lightDirection, viewDirection, mat1
			) * strength * float(shadow >= SPECULAR_SHADOW_CUTOFF);
		}

		float shadow = 0.0;

		//TODO
		calculateShadows(14, pFragmentFlashlightLightPos, normalizedNormal, directionalLightDirection);
		calculateShadows(13, pFragmentRightFrontLightPos, normalizedNormal, directionalLightDirection);
		calculateShadows(12, pFragmentLeftFrontLightPos, normalizedNormal, directionalLightDirection);

		lightingMultiplier = clamp(max(uAmbientLight + lighting + lightingSpecular, mat1.brightness/255.0), 0.0, 1.0);
	}
	else {
		lightingMultiplier = vec3(1.0);
	}

	if(uWriteToRenderTargets == 1) {
		//solid
		oColor = vec4(baseColor.rgb * lightingMultiplier, 1.0);
	}
	else if(uWriteToRenderTargets == 2) {
		//transparent

		//some random weight function
		//just gets how much color
		float weight = clamp(pow(min(1.0, baseColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

		oColor = vec4(baseColor.rgb * lightingMultiplier * baseColor.a, baseColor.a) * weight;
		oReveal = baseColor.a;
	}
	else {
		oColor = vec4(baseColor.xyz * lightingMultiplier, baseColor.w);
		//oColor = vec4(1.0, 0.0, 1.0, 1.0); //debug purple
	}
};
