#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform bool isEmissive;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 emissive_color;


void main() {
	if (isEmissive) {
		color = vec4(0.0); // no lighting
		emissive_color = vec4(objectColor, 1.0);
	} else {
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
		emissive_color = vec4(0.0);
	}
}
