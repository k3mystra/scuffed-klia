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
    bool isDirty = true;

    glm::mat4 matrix = glm::mat4(1);
    glm::mat4 invMatrix = glm::mat4(1);
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::quat();
    glm::vec3 scale = glm::vec3(0);
};

struct Material {
    unsigned int textureID = 0;
    glm::vec3 color = COLOR::RED;
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
    std::vector<unsigned int> faceIndices = std::vector<unsigned int>();
};

struct Model {
    Shader shader = Shader();
    std::vector<Mesh> meshes;
    std::string srcPath = "";
};

struct AmbientLight {
    float intensity = 1.0;
    glm::vec3 color = COLOR::WHITE;
};

struct SunLight {
    float intensity = 1.2;
    glm::vec3 color = COLOR::WHITE;
    glm::vec3 direction = glm::vec3(-0.1961, -0.7845, 0.5883);
};

struct Skybox {};

struct Camera {
    float fov = 45;
    float aspectRatio = 16.0/9.0;
    float nearPlane = 0.5;
    float farPlane = 1000;

    glm::mat4 projectionMatrix = glm::mat4(1);
};
