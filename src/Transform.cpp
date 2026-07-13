#include "Transform.h"

#include <string>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>


void recalcTransform(Transform& transform) {
    transform.matrix = glm::mat4(1);

    transform.matrix = glm::translate(transform.matrix, transform.position);
    transform.matrix = transform.matrix * glm::toMat4(transform.rotation);
    transform.matrix = glm::scale(transform.matrix, transform.scale);

    transform.invMatrix = glm::inverse(transform.matrix);

    transform.isDirty = false;
}

void updateTransform(World& world) {
    for (Transform& transform : world.transformList) {
        recalcTransform(transform);
    }
}
