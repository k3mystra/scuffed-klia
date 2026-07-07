#include "Transform.h"

#include <string>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

static void recalcTransform(Transform& transform);


void transformSystemInit(WorldState& worldState) {
    std::ifstream& sceneFile = worldState.sceneConfig;

    sceneFile.seekg(0);

    std::string line;
    while (std::getline(sceneFile, line)) {
        if (line[0] != '#')
            continue;

        // TODO: get the offset

        Transform transform = Transform();
        transform.isDirty = false;
        transform.matrix = glm::mat4(1);

        // Line format: position vector, rotation (quaternion: w, x, y, z ), scale vector
        // Total: 3 + 4 + 3 = 10

        transform.position = glm::vec3(0);
        for (int i = 0; i < glm::vec3::length(); i++)
            sceneFile >> transform.position[i];

        transform.rotation = glm::quat(0, 0, 0, 0);
        for (int i = 0; i < glm::quat::length(); i++)
            sceneFile >> transform.rotation[i];

        transform.scale = glm::vec3(0);
        for (int i = 0; i < glm::vec3::length(); i++)
            sceneFile >> transform.scale[i];

        recalcTransform(transform);

        worldState.transformList.push_back(transform);
    }
}

static void recalcTransform(Transform& transform) {
    transform.matrix = glm::mat4(1);

    transform.matrix = glm::translate(transform.matrix, transform.position);
    transform.matrix = transform.matrix * glm::toMat4(transform.rotation);
    transform.matrix = glm::scale(transform.matrix, transform.scale);

    transform.invMatrix = glm::inverse(transform.matrix);

    transform.isDirty = false;
}
