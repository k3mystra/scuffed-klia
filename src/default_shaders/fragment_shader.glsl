#version 330 core

uniform vec3 matColor;
uniform vec3 ambientLightColor;
uniform vec3 sunLightColor;
uniform vec3 sunLightDir;
uniform bool hasTexture;
uniform sampler2D diffuseTexture;
uniform float opacity;
uniform vec3 viewPos; // Camera position

in vec3 faceNormal;
in vec2 geoTexCoord;
in vec3 fragWorldPos; // World space position of fragment
out vec4 color;

void main()
{
    vec3 baseColor = hasTexture ? vec3(texture(diffuseTexture, geoTexCoord)) : matColor;

    // Increased ambient contribution (35%) so models aren't dark
    vec3 ambientColor = ambientLightColor * 0.35;

    // Diffuse component
    vec3 normal = normalize(faceNormal);
    float diff = max(dot(-normalize(sunLightDir), normal), 0.0);
    vec3 sunLight = min(diff * sunLightColor, 1.0);

    // Specular component (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 lightDir = normalize(-sunLightDir);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); // 32.0 is shininess
    vec3 specularColor = spec * sunLightColor * 0.3; // 0.3 is specular strength

    color = vec4((ambientColor + sunLight) * baseColor + specularColor, opacity);
}
