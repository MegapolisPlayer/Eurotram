#version 450 core

//vertex shader for rendering texture to screen

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoords;

out vec2 pTexCoords;

void main() {
	pTexCoords = iTexCoords;
	gl_Position = vec4(iPos.x, iPos.y, 1.0, 1.0);
};
