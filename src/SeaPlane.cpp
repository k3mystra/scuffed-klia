#include "SeaPlane.h"

SeaPlane::SeaPlane(int gridSize, float cellSize) : GameObject() {
    generate(gridSize, cellSize);
}

void SeaPlane::generate(int gridSize, float cellSize) {
    float half = gridSize * cellSize / 2.0f;
    float uvScale = 50.0f; // how many times the texture tiles

    for (int z = 0; z <= gridSize; z++) {
        for (int x = 0; x <= gridSize; x++) {
            float px = -half + x * cellSize;
            float pz = -half + z * cellSize;

            float u = (float)x / gridSize * uvScale;
            float v = (float)z / gridSize * uvScale;

            // Position (3) + UV (2) + Normal (3) = 8 floats
            vertices.push_back(px);
            vertices.push_back(0.0f);  // Y = 0, sea level
            vertices.push_back(pz);
            vertices.push_back(u);
            vertices.push_back(v);
            vertices.push_back(0.0f);  // normal X
            vertices.push_back(1.0f);  // normal Y (pointing up)
            vertices.push_back(0.0f);  // normal Z
        }
    }

    // Generate indices for triangle grid
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int topLeft     = z * (gridSize + 1) + x;
            int topRight    = topLeft + 1;
            int bottomLeft  = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

const std::vector<float>& SeaPlane::getVertices() const { return vertices; }
const std::vector<unsigned int>& SeaPlane::getIndices() const { return indices; }