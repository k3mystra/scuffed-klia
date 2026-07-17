#pragma once

#include "Components.h"
#include "World.h"

namespace transform_utils {
    void recalcTransform(Transform& transform);
    void setPosition(Transform& transform, glm::vec3 position);
    void setRotation(Transform& transform, glm::quat rotation);
    void setScale(Transform& transform, glm::vec3 scale);
};

void updateTransform(World& world);
