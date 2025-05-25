#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D non_emissive_texture;
uniform sampler2D emissive_texture;

void main(){
    vec3 base = texture(non_emissive_texture, UV).rgb;
    vec3 glow = texture(emissive_texture, UV + 0.005*vec2( sin(0.2+1024.0*UV.x),cos(0.2+768.0*UV.y))).rgb;

    // Simple additive blend
    vec3 color = base + glow;

    // Optionally clamp or tone-map
    FragColor = vec4(color, 1.0);
}