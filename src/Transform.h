#pragma once

#include "Components.h"
#include "World.h"

namespace transform_utils {
    void recalcTransform(Transform& transform);
};

void updateTransform(World& world);
