#version 410 

in vec4 fragNormal;
in vec4 fragPosition;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 materialColor;
uniform vec3 ambientLight;

uniform vec3 eyePos;
uniform float specularPower;

void main()
{
	vec3 N = normalize(fragNormal.xyz);
	vec3 L = normalize(lightDir);

	vec3 ambient = materialColor * ambientLight;

	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * lightColor * materialColor;

	vec3 E = normalize(eyePos - fragPosition.xyz);
	vec3 R = reflect(L, N);
	float s = max(0.0, dot(R, E));
	s = pow(s, specularPower);
	vec3 specular = vec3(s) * lightColor * materialColor;

	fragColor = vec4(ambient + diffuse + specular, 1);
}