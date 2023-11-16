#version 410

/*
Fragment Program for GraphicsDrawer objects
*/
in vec4 oColor;
in vec2 oUV;

out vec4 fragColor;

uniform sampler2D image;

void main() {
	fragColor = texture(image, oUV) * oColor;
}
