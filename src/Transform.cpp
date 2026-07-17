#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace transform_utils {
    void recalcTransform(Transform& transform) {
        if (!transform.isDirty)
            return;

        // Re-normalize quaternion to avoid weird scaling due to floating point error
        // after numerous rotations
        transform.rotation = glm::normalize(transform.rotation);

        transform.matrix = glm::translate(glm::mat4(1.0f), transform.position)
                  * glm::toMat4(transform.rotation)
                  * glm::scale(glm::mat4(1.0f), transform.scale);
        transform.invMatrix = glm::inverse(transform.matrix);

        transform.isDirty = false;
    }

    void setPosition(Transform& transform, glm::vec3 position) {
        transform.position = position;
        transform.isDirty = true;
    }

    void setRotation(Transform& transform, glm::quat rotation) {
        transform.rotation = rotation;
        transform.isDirty = true;
    }

    void setScale(Transform& transform, glm::vec3 scale) {
        transform.scale = scale;
        transform.isDirty = true;
    }
}

void updateTransform(World& world) {
    for (Transform& transform : world.transformList) {
        transform_utils::recalcTransform(transform);
    }
}
