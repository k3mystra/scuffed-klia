#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 fragTexCoord;
out vec3 vertNormal;
out vec3 worldPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    fragTexCoord = texCoord;
    vertNormal = mat3(transpose(inverse(model))) * normal;
    worldPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(worldPos, 1.0);
}