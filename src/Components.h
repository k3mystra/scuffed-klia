#ifndef Components
#define Components

#include <vector>
#include <string>

#include <glm/glm.hpp>


struct EntityData {
    std::string name;
};

struct Transform {
    bool isDirty;

    glm::mat4 transform;
    glm::mat4 invTransform;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct Material {
    glm::vec3 color;
    std::string diffuseTexturePath = "";
    unsigned int textureID = 0;
};

struct Mesh {
    Material material;
    unsigned int shaderProgramID = 0;

    std::string vertexShaderSrcPath = "";
    std::string geometryShaderSrcPath = "";
    std::string fragmentShaderSrcPath = "";
};

struct Model {
    std::vector<Mesh> meshes;
};

struct Light {
    enum Type {
        Ambient, Sunlight, Spotlight
    };

    Type type;
    float intensity;

    glm::vec3 color;
    glm::vec3 direction;
};

struct Skybox {};

#endif
