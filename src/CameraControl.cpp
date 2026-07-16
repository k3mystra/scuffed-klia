#include "CameraControl.h"

#include "InputManager.h"
#include "Transform.h"

#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <vector>

const float MOVE_SPEED = 10;
const float PAN_SPEED = 5;

static void removeVectorElevation(glm::vec3 vec) {
    vec.y = 0.0;
    vec = glm::normalize(vec);
}

// Minecraft-like control
static void handleAxisMovement(Transform* transform, float deltaTime) {
    glm::vec3 moveInput = glm::vec3(0.0);
    float elevationInput = 0.0;

    if (InputManager::isKeyPressed(GLFW_KEY_W))
        moveInput.z = 1;
    else if (InputManager::isKeyPressed(GLFW_KEY_S))
        moveInput.z = -1;

    if (InputManager::isKeyPressed(GLFW_KEY_A))
        moveInput.x = -1;
    else if (InputManager::isKeyPressed(GLFW_KEY_D))
        moveInput.x = 1;

    if (InputManager::isKeyPressed(GLFW_KEY_SPACE))
        elevationInput = 1;
    else if (InputManager::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        elevationInput = -1;

    glm::vec3 localForwardVector = transform->matrix[2];
    glm::vec3 localRightVector = transform->matrix[0];

    // Remove elevation added by Y-axis magnitude
    // Avoid elevation change being applied on moveInput X-Z field-of-view plane
    // TL;DR: making this feels like Minecraft control
    removeVectorElevation(localForwardVector);
    removeVectorElevation(localRightVector);

    glm::vec3 moveVector = (localRightVector * moveInput.x) + (-localForwardVector * moveInput.z);
    moveVector *= MOVE_SPEED * deltaTime;
    // Apply elevation
    moveVector.y = elevationInput * MOVE_SPEED * deltaTime;

    glm::vec3 camPos = transform->position;

    transform_utils::setPosition(*transform, camPos + moveVector);
}

static void handleCursorPanning(Transform* transform, const InputEvent& event, float deltaTime) {
    // glm::quat panRotation = glm::quat(glm::vec3(-event.mousePosDelta.y, -event.mousePosDelta.x, 0.0) * PAN_SPEED);

    // Divide by 1000 cuz mousePosDelta is pixel-distance, and that's BIG
    float upDownDelta = -event.mousePosDelta.y * PAN_SPEED * deltaTime / 100;
    float leftRightDelta = -event.mousePosDelta.x * PAN_SPEED * deltaTime / 100;

    glm::quat panRotation = glm::angleAxis(leftRightDelta, glm::vec3(0.0, 1.0, 0.0));

    glm::vec3 localRightAxis = transform->rotation * glm::vec4(1.0, 0.0, 0.0, 1.0);
    panRotation = glm::angleAxis(upDownDelta, localRightAxis) * panRotation;

    transform_utils::setRotation(*transform, panRotation * transform->rotation);
}

void processCameraControl(World& world) {
    // Find camera transform data
    Transform* transform = nullptr;
    for (int e = 0; e < world.totalEntity; e++) {
        auto camSearch = world.cameraIndex.find(e);
        if (camSearch == world.cameraIndex.end())
            continue;

        auto transformSearch = world.transformIndex.find(e);
        if (transformSearch == world.transformIndex.end())
            continue;

        transform = &world.transformList[transformSearch->second];
        break;
    }

    if (transform == nullptr) {
        std::cerr << "No transform found for camera\n";
        return;
    }

    handleAxisMovement(transform, world.deltaTime);

    const InputEventQueue& inputEventQueue = InputManager::getInputQueue();

    for (const InputEvent& event: inputEventQueue) {
        switch (event.type) {
            case InputEvent::Type::MouseMove:
                handleCursorPanning(transform, event, world.deltaTime);
                break;
            default:
                continue;
        }
    }
}

