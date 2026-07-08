#ifndef Components
#define Components

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>


struct EntityData {
    std::string name;
};

struct Transform {
    bool isDirty = false;

    glm::mat4 matrix;
    glm::mat4 invMatrix;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct Material {
    glm::vec3 color;
    std::string diffuseTexturePath = "";
    unsigned int textureID = 0;
};

struct Mesh {
    unsigned int shaderProgramID = 0;
    Material material;

    std::string vertexShaderSrcPath = "";
    std::string geometryShaderSrcPath = "";
    std::string fragmentShaderSrcPath = "";

    std::vector<float> vertices = std::vector<float>();
    std::vector<float> faceIndices = std::vector<float>();
};

struct Model {
    std::vector<Mesh> meshes;
    std::string srcPath = "";
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
