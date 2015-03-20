#version 410

in vec4 position;
in vec4 normal;
out vec4 fragNormal;
out vec4 fragPosition;

uniform mat4 projView;

void main()
{
	fragPosition = position;
	fragNormal = normal;
	gl_Position = projView * position;
}