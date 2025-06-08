#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 uv_modelspace;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
    FragPos = vec3(M * vec4(vertexPosition_modelspace, 1.0));
    Normal = mat3(transpose(inverse(M))) * vertexNormal_modelspace;
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    TexCoord = uv_modelspace;
}
