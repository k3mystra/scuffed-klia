#ifndef Components
#define Components

#include <vector>
#include <string>

#include <glm/fwd.hpp>
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

const glm::vec3 BLACK_COLOR = glm::vec3(0);

struct Material {
    unsigned int textureID = 0;
    glm::vec3 color = BLACK_COLOR;
    std::string diffuseTexturePath = "";
};

struct Shader {
    unsigned int shaderProgramID = 0;

    std::string vertexShaderSrcPath = "";
    std::string geometryShaderSrcPath = "";
    std::string fragmentShaderSrcPath = "";
};

struct Mesh {
    Material material = Material();
    Shader shader = Shader();

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
