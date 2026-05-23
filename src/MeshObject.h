#pragma once

#include "GameObject.h"
#include <glm/ext/vector_float3.hpp>
#include <vector>

struct MeshBufferInfo {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

struct Material {
    glm::vec3 color;
};


class MeshObject : public GameObject {
public:
    MeshObject();
    ~MeshObject();

    MeshBufferInfo bufferInfo;
    Material material;

    // Placeholder for mesh data
    const std::vector<float>& getVertices() const;
    void setVertices(const std::vector<float>& newVertices);

    const std::vector<unsigned int>& getIndices() const;
    void setIndices(const std::vector<unsigned int>& newIndices);

    // void loadSimpleObj(const std::string& path);
    
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
