///////////////////////////////////////////////////////
/// SIMPLE DIFFUSE COLOR VERTEX PROGRAM ////////////
///////////////////////////////////////////////////////

varying vec4 oColor;
varying vec2 oUV;

uniform sampler2D diffuseMap; // TEXUNIT0

uniform vec3 ambientColor;

void main() {
//	gl_FragColor = vec4(ambientColor, 1.0) * texture2D(diffuseMap, oUV) * oColor;
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) * texture2D(diffuseMap, oUV) * oColor;
}
