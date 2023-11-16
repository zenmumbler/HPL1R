// Vertex func for all purpose 3D scene drawing
#version 410

layout(location = 0) in vec4 position;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 uv;

out vec4 oColor;
out vec2 oUV;

uniform mat4 worldViewProj;

void main() {
	gl_Position = worldViewProj * position;
	oColor = color;
	oUV = uv;
}
