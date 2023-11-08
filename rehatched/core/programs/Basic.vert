/*
Vertex Program for GraphicsDrawer element
*/
attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;

varying vec4 oColor;
varying vec2 oUV;

uniform mat4 worldViewProj;

void main() {
	gl_Position = worldViewProj * vec4(position, 1.0);
	oColor = color;
	oUV = uv;
}
