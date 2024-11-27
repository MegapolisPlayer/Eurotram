#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
uniform mat4 umatrix;
out vec2 otexcoord;
void main() {
	gl_Position = umatrix * vec4(Position, 1.0);
	otexcoord = TexCoord;
};
