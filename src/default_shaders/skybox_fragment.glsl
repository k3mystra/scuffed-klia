#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    vec3 coords = TexCoords;
    if (TexCoords.y > abs(TexCoords.x) && TexCoords.y > abs(TexCoords.z)) {
        // Rotates the top face texture. You can change this line to try different rotations:
        //   Option A ( 90 deg CCW): vec3(-TexCoords.z, TexCoords.y,  TexCoords.x)
        //   Option B (-90 deg CW) : vec3( TexCoords.z, TexCoords.y, -TexCoords.x)
        //   Option C (180 deg)    : vec3(-TexCoords.x, TexCoords.y, -TexCoords.z)
        coords = vec3(-TexCoords.x, TexCoords.y, -TexCoords.z);
    }
    
    FragColor = texture(skybox, coords);
}
