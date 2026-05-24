// Model.h
#pragma once
#include "GameObject.h"
#include "MeshObject.h"
#include <vector>

class Model : public GameObject {
public:
    Model();

    std::vector<MeshObject> meshes; // one per OBJ shape/group

    void recalcTransform();
};
