#version 330 core
layout(location = 0) in vec3 vertexPosition;
uniform mat4 VP;

void main() {
    gl_Position = VP * vec4(vertexPosition, 1.0);
}
