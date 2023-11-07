/*
Vertex Program for GraphicsDrawer element
*/
attribute vec4 position;
attribute vec4 color;
attribute vec2 uv;

varying vec4 oColor;
varying vec2 oUV;

uniform mat4 worldViewProj;

void main() {
	gl_Position = worldViewProj * position;
	oColor = color;
	oUV = uv;
}
