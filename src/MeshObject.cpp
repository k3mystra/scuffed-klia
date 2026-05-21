#include "MeshObject.h"

MeshObject::MeshObject() : GameObject() {}

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
