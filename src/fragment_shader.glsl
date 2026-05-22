#version 330 core

uniform vec3 matColor;
out vec4 color;

void main()
{
    color = vec4(matColor, 1.0);
}
