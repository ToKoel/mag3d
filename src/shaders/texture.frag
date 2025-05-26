#version 330 core

out vec4 FragColor;

uniform sampler2D non_emissive_texture;
uniform sampler2D emissive_texture;
uniform vec2 tex_size;

void main(){
    vec3 base = texture(non_emissive_texture, gl_FragCoord.xy / tex_size).rgb;
    vec3 glow = texture(emissive_texture, gl_FragCoord.xy / tex_size).rgb;
    //vec3 glow = texture(emissive_texture, textureCoords + 0.005*vec2( sin(0.2+1024.0*textureCoords.x),cos(0.2+768.0*textureCoords.y))).rgb;

    vec3 color = base + glow;

    FragColor = vec4(color, 1.0);
}