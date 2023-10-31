/*
Vertex Program for GraphicsDrawer element
*/

varying vec4 oColor;
varying vec2 oUV;

uniform sampler2D diffuseMap;

void main() {
	gl_FragColor = vec4(ambientColor, 1.0) * texture2D(diffuseMap, oUV) * oColor;
}
