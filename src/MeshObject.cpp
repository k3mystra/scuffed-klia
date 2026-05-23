#include <fstream>
#include <sstream>
#include <string>
#include <vector>
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

// void MeshObject::loadSimpleObj(const std::string& path) {
//     std::ifstream file(path);
//     std::string line;
//     std::vector<float> rawVertices;
//     std::vector<unsigned int> faceIndices;

//     while (std::getline(file, line)) {
//         std::stringstream ss(line);
//         std::string prefix;
//         ss >> prefix;

//         if (prefix == "v") {
//             float x, y, z;
//             ss >> x >> y >> z;
//             rawVertices.push_back(x); rawVertices.push_back(y); rawVertices.push_back(z);
//         } 
//         else if (prefix == "f") {
//             std::string vertexData;
//             while (ss >> vertexData) {
//                 // Obj format is v/vt/vn, we only want the 'v' part (index)
//                 size_t slashPos = vertexData.find('/');
//                 int vIdx = std::stoi(vertexData.substr(0, slashPos)) - 1; // -1 because OBJ is 1-indexed
//                 faceIndices.push_back(vIdx);
//             }
//         }
//     }
    
//     setVertices(rawVertices);
//     setIndices(faceIndices);
// }