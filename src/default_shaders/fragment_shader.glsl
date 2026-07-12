#version 330 core

uniform vec3 matColor;
uniform vec3 ambientLightColor;
uniform vec3 sunLightColor;
uniform vec3 sunLightDir;
uniform bool hasTexture;
uniform sampler2D diffuseTexture;

in vec3 faceNormal;
in vec2 geoTexCoord;
out vec4 color;

void main()
{
    vec3 baseColor = hasTexture ? vec3(texture(diffuseTexture, geoTexCoord)) : matColor;

    vec3 ambientColor = ambientLightColor * 0.1;

    vec3 normal = normalize(faceNormal);
    float diff = max(dot(-normalize(sunLightDir), normal), 0.0);
    vec3 sunLight = min(diff * sunLightColor, 1.0);

    color = vec4((ambientColor + sunLight) * baseColor, 1.0);
}
