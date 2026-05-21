#pragma once

#include "GameObject.h"
#include <vector>

class MeshObject : public GameObject {
public:
    MeshObject();
    ~MeshObject();

    // Placeholder for mesh data
    const std::vector<float>& getVertices() const;
    void setVertices(const std::vector<float>& newVertices);

    const std::vector<unsigned int>& getIndices() const;
    void setIndices(const std::vector<unsigned int>& newIndices);

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
