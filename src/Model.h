// Model.h
#pragma once
#include "GameObject.h"
#include "MeshObject.h"
#include <vector>

class Model : public GameObject {
public:
    std::vector<MeshObject> meshes; // one per OBJ shape/group

    void recalcTransform() {
        GameObject::recalcTransform(); // recalc own transform
        // propagate to all meshes as local offset
        for (auto& mesh : meshes) {
            mesh.recalcTransform();
        }
    }
};