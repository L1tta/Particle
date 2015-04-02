#version 410

layout( location = 0 ) in vec4 position;
layout( location = 1 ) in vec4 normal;

out vec4 fragPosition;
out vec4 fragNormal;

uniform mat4 projectionView;
uniform mat4 model;
uniform mat4 invTransposeModel;

void main() {

	fragPosition = model * position;
	fragNormal = invTransposeModel * normal;
	gl_Position = projectionView * fragPosition;
}