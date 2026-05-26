#pragma once
#include "GameObject.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct SeaBufferInfo {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

class SeaPlane : public GameObject {
public:
    SeaPlane(int gridSize = 100, float cellSize = 10.0f);

    SeaBufferInfo bufferInfo;
    unsigned int diffuseTextureID = 0;
    unsigned int normalTextureID = 0;

    const std::vector<float>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    void generate(int gridSize, float cellSize);
};