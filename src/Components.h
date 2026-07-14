#pragma once

#include <vector>
#include <string>

#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Color.h"


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
    unsigned int textureID = 0;
    glm::vec3 color = COLOR::BLACK;
    std::string diffuseTexturePath = "";
};

struct Shader {
    unsigned int programID = 0;

    std::string vertexShaderSrcPath = "";
    std::string geometryShaderSrcPath = "";
    std::string fragmentShaderSrcPath = "";
};

struct Mesh {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    Material material = Material();

    std::vector<float> vertices = std::vector<float>();
    std::vector<float> faceIndices = std::vector<float>();
};

struct Model {
    Shader shader = Shader();
    std::vector<Mesh> meshes;
    std::string srcPath = "";
};

struct AmbientLight {
    float intensity = 0.2;
    glm::vec3 color = COLOR::GREY;
};

struct SunLight {
    float intensity = 0.5;
    glm::vec3 color = COLOR::WHITE;
    glm::vec3 direction = glm::vec3(0.5, 0.5, 0.5);
};

struct Skybox {};

struct Camera {
    float fov = 45;
    float aspectRatio = 16.0/9.0;
    float nearPlane = 0.5;
    float farPlane = 1000;

    glm::mat4 projectionMatrix = glm::mat4(1);
};
