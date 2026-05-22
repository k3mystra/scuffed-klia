#version 330 core
flat in int colorID;
out vec4 color;
const vec3 colors[8] = vec3[](
    vec3(0.0, 0.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 1.0)
);

void main()
{
    vec3 choice;
    choice = colors[colorID % 8];
    color = vec4(choice, 1.0);
}
