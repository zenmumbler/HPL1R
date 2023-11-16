#version 410
/*
Vertex Program for GraphicsDrawer objects
*/
in vec3 position;
in vec4 color;
in vec2 uv;

out vec4 oColor;
out vec2 oUV;

uniform mat4 projection;

void main() {
	gl_Position = projection * vec4(position, 1.0);
	oColor = color;
	oUV = uv;
}
