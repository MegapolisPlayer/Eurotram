#version 330 core
out vec4 Color;
in vec2 otexcoord;
uniform sampler2D tex;
void main() {
	Color = texture(tex, otexcoord);
};
