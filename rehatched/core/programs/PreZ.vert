// Vertex func for Pre-Z pass
#version 410

layout(location = 0) in vec4 position;

uniform mat4 worldViewProj;

void main() {
	gl_Position = worldViewProj * position;
}
