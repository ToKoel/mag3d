#version 330 core
out vec4 FragColor;
uniform sampler2D scene_texture;
uniform float threshold = 1.0;
uniform vec2 tex_size;

void main() {
    vec2 uv = gl_FragCoord.xy / tex_size;
    vec3 color = texture(scene_texture, uv).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); // luminance
    if (brightness > threshold)
    FragColor = vec4(color, 1.0);
    else
    FragColor = vec4(0.0);
}