#include "Model.h"


Model::Model() : GameObject() {};

void Model::recalcTransform() {
    recalcTransform(); // recalc own transform
    // propagate to all meshes as local offset
    for (auto& mesh : meshes) {
        mesh.recalcTransform();
    }
}
