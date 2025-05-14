#version 330 core

//in vec3 fragmentColor;

// Ouput data
out vec4 color;

uniform vec3 objectColor;  // e.g. passed in from C++ as (1.0, 0.0, 0.0)


void main()
{

	color = vec4(objectColor, 1.0);

}