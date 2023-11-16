#version 410

in vec4 position;
in vec4 color;
in vec2 uv;

out vec4 oColor;
out vec2 oUV;

uniform mat4 worldViewProj;

void main() {
	gl_Position = worldViewProj * position;
	oColor = color;
	oUV = uv;
}
