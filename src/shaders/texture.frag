#version 330 core

out vec4 FragColor;

uniform sampler2D non_emissive_texture;
uniform sampler2D emissive_texture;
uniform vec2 tex_size;

void main(){
    vec2 uv = gl_FragCoord.xy / tex_size;
    vec3 base = texture(non_emissive_texture, uv).rgb;


   float offset = 1.0 / 200.0; // smaller = sharper glow
    vec3 glow = vec3(0.0);

    // Sample a 3x3 grid around the current pixel
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 offset_uv = gl_FragCoord.xy/tex_size + vec2(x, y) * offset;
            glow += texture(emissive_texture, offset_uv).rgb;
        }
    }

    glow /= 9.0; // average

    vec3 color = base + glow;

    FragColor = vec4(color, 1.0);
}