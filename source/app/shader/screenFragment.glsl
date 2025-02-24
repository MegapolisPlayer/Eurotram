#version 450 core

//fragment shader for rendering texture to screen

in vec2 pTexCoords;
out vec4 oColor;

layout(location = 50) uniform sampler2D uRenderData;

void main() {
	oColor = vec4(texture(uRenderData, pTexCoords).rgb, 1.0);
};
