#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 color;

void main() {
	// Ambient
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Combine
	vec3 result = (ambient + diffuse) * objectColor;
	color = vec4(result, 1.0);
}
