#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 fragTexCoord[];
in vec3 vertNormal[];
in vec3 worldPos[];

out vec3 faceNormal;
out vec2 geoTexCoord;

void main() {
    // Compute flat normal in world space
    vec3 e1 = worldPos[1] - worldPos[0];
    vec3 e2 = worldPos[2] - worldPos[0];
    faceNormal = normalize(cross(e1, e2));

    // If you want smooth normals instead, comment above and use:
    // faceNormal = normalize(vertNormal[0] + vertNormal[1] + vertNormal[2]);

    gl_Position = gl_in[0].gl_Position;
    geoTexCoord = fragTexCoord[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    geoTexCoord = fragTexCoord[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    geoTexCoord = fragTexCoord[2];
    EmitVertex();

    EndPrimitive();
}