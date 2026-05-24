// Model.h
#pragma once
#include "GameObject.h"
#include "MeshObject.h"
#include "Physics.h"
#include <vector>

class Model : public GameObject {
public:
    Model();

    bool isPhysicsEnabled;
    PhysicsState physicsState;

    std::vector<MeshObject> meshes; // one per OBJ shape/group

    void recalcTransform();
};
