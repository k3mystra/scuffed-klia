#include "Model.h"

#include <iostream>


namespace model_utils {
    void printModel(Model model) {
        std::cout << "Mesh count: " << model.meshes.size() << '\n';
        for (const Mesh& mesh : model.meshes) {
            std::cout << "[";
            std::cout << "Vertices: " << mesh.vertices.size();
            std::cout << " Faces: " << mesh.faceIndices.size();
            std::cout << "]\n";
        }
    }
};
