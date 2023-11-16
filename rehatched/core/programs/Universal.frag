#version 410

///////////////////////////////////////////////////////
/// SIMPLE DIFFUSE COLOR VERTEX PROGRAM ////////////
///////////////////////////////////////////////////////

in vec4 oColor;
in vec2 oUV;

out vec4 fragColor;

uniform sampler2D diffuseMap; // TEXUNIT0
uniform vec3 ambientColor;

void main() {
	// fragColor = vec4(ambientColor, 1.0) * texture(diffuseMap, oUV) * oColor;
	fragColor = texture(diffuseMap, oUV) * oColor;
}
