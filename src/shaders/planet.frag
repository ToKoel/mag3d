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
uniform sampler2D planetNightTexture;
uniform bool useTexture;
uniform bool hasNightTexture;

layout(location = 0) out vec4 color;


void main() {
	if (isEmissive) {
		color = vec4(objectColor * 4.0f, 1.0);
	} else {
		// Ambient
		float ambientStrength = 0.3;
		vec3 ambient = ambientStrength * lightColor;

		// Diffuse
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		float diffuseStrength = 2.0;
		vec3 diffuse = diff * lightColor * diffuseStrength;

		// Combine
		vec3 result = (ambient + diffuse) * objectColor;

		if (useTexture) {
			vec2 correctedUV = vec2(TexCoord.x, 1.0 - TexCoord.y);
			vec3 dayColor = texture(planetTexture, correctedUV).rgb;
			vec3 textures = dayColor;
			if (hasNightTexture) {
				vec3 nightColor = texture(planetNightTexture, correctedUV).rgb;
				// Blend between day and night map
				textures = mix(nightColor * 2.0, dayColor, diff);
			}
			result = (ambient + diffuse) * textures;
		}


		color = vec4(result, 1.0);
		if (selected){
			vec4 outlineColor = vec4(0.1, 0.9, 0.0, 0.4);
			color = vec4(mix(result, outlineColor.rgb, outlineColor.a), 1.0);
			return;
		}
	}
}
