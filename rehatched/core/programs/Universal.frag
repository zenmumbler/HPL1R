// Fragment func for all purpose 3D scene drawing
#version 410

in vec4 oColor;
in vec2 oUV;

layout(location = 0) out vec4 fragColor;

uniform sampler2D diffuseMap;
uniform vec3 ambientColor;

void main() {
	// fragColor = vec4(ambientColor, 1.0) * texture(diffuseMap, oUV) * oColor;
	vec4 diffuse = texture(diffuseMap, oUV);
	// this test matches the alpha test from RenderState
	if (diffuse.a < 0.6) discard;
	fragColor = diffuse * oColor;
}
