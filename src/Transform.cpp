#include "Transform.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

namespace transform_utils {
    void recalcTransform(Transform& transform) {
        transform.matrix = glm::translate(glm::mat4(1.0f), transform.position)
                  * glm::toMat4(transform.rotation)
                  * glm::scale(glm::mat4(1.0f), transform.scale);
        transform.invMatrix = glm::inverse(transform.matrix);

        transform.isDirty = false;
    }
}

void updateTransform(World& world) {
    for (Transform& transform : world.transformList) {
        transform_utils::recalcTransform(transform);
    }
}
