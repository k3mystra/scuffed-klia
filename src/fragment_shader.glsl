#version 330 core

uniform vec3 matColor;
uniform vec3 ambientLightColor;
uniform vec3 sunLightColor;
uniform vec3 sunLightDir;
in vec3 faceNormal;
out vec4 color;

void main()
{
    // "Multiply" here is by component, NOT dot product
    // Small ambient factor
    vec3 ambientColor = ambientLightColor * 0.1;
    // Get the "perpendicularity" of the light and multiply by light color
    vec3 sunLight = max(dot(normalize(sunLightDir), normalize(faceNormal)), 0.0) * sunLightColor;
    sunLight = min(sunLight, 1.0);
    color = vec4((ambientColor + sunLight) * matColor, 1.0);
}
