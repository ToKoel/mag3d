#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform bool isEmissive;
uniform bool selected;
uniform sampler2D planetTexture;
uniform bool useTexture;

layout(location = 0) out vec4 color;


void main() {
	vec3 baseColor = useTexture
	? texture(planetTexture, TexCoord).rgb
	: objectColor;

	if (isEmissive) {
		color = vec4(baseColor * 2.0f, 1.0);
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
		vec3 result = (ambient + diffuse) * baseColor;

		color = vec4(result, 1.0);
		if (selected){
			vec4 outlineColor = vec4(0.1, 0.9, 0.0, 0.4);
			color = vec4(mix(result, outlineColor.rgb, outlineColor.a), 1.0);
			return;
		}
	}
}
