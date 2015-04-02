#version 410

in vec4 fragPosition;
in vec4 fragNormal;

layout( location = 0 ) out vec4 fragColor;

void main() {

	vec3 color = vec3(1);

	// grid every 1-unit
	if (frac fragPosition.x < 0.05f ||
		frac fragPosition.y < 0.05f ||
		frac fragPosition.z < 0.05f)
		color = vec3(0);

	// fake light
	float d = max( 0, dot( normalize(vec3(1,1,1)),
			normalize(fragNormal.xyz) ) ) * 0.75f;
	fragColor.rgb = colour * 0.25f + colour * d;

	fragColor.a = 1;
}