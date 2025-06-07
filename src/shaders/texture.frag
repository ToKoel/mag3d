#version 330 core

out vec4 FragColor;

uniform sampler2D scene_texture;
uniform sampler2D bloom_texture;
uniform sampler2D path_texture;
uniform bool enable_bloom;
uniform float bloom_strength;
uniform vec2 tex_size;
uniform bool show_paths;

void main() {
    vec2 uv = gl_FragCoord.xy / tex_size;
    vec3 scene_color = texture(scene_texture, uv).rgb;
    vec3 bloom_color = texture(bloom_texture, uv).rgb;
    vec3 path_color = texture(path_texture, uv).rgb;

    vec3 color = scene_color;
    if (show_paths) color = path_color;
    if (enable_bloom) color += bloom_strength * bloom_color;

    float exposure = 1.0f;
    float gamma = 1.0f;
    vec3 result = vec3(1.0f) - exp(-color * exposure);

    FragColor.rgb = pow(result, vec3(1.0f / gamma));
}
