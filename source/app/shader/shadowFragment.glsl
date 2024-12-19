#version 450 core

//simple shader to save info to depth texture

void main() {
	gl_FragDepth = gl_FragCoord.z; //if new material closer, overwrite
}
