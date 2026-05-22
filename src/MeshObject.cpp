#include "MeshObject.h"
#include <glm/ext/vector_float3.hpp>

MeshObject::MeshObject() : GameObject() {
    material = Material();
    material.color = glm::vec3(0.0);
}

MeshObject::~MeshObject() {}

const std::vector<float>& MeshObject::getVertices() const {
    return vertices;
}

void MeshObject::setVertices(const std::vector<float>& newVertices) {
    vertices = newVertices;
}

const std::vector<unsigned int>& MeshObject::getIndices() const {
    return indices;
}

void MeshObject::setIndices(const std::vector<unsigned int>& newIndices) {
    indices = newIndices;
}
