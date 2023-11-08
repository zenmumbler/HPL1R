/*
Fragment Program for GfxObjects
*/

varying vec4 oColor;
varying vec2 oUV;

uniform sampler2D diffuseMap;

void main() {
	gl_FragColor = texture2D(diffuseMap, oUV) * oColor;
}
