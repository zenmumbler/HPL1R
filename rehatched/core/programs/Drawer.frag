/*
Fragment Program for GraphicsDrawer objects
*/

varying vec4 oColor;
varying vec2 oUV;

uniform sampler2D image;

void main() {
	gl_FragColor = texture2D(image, oUV) * oColor;
}
