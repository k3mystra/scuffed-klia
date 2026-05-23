#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 faceNormal;

void main() {
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;

    // Actually calculate the face normal
    vec3 e1 = vec3(v1 - v0);
    vec3 e2 = vec3(v2 - v0);
    faceNormal = normalize(cross(e1, e2));

    // Emit vertex AFTER calculation to ensure face normal is emitted
    gl_Position = v0;
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();

    EndPrimitive();
}
