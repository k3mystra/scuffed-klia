#include "CameraControl.h"

#include "InputManager.h"
#include "Transform.h"

#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>
#include <iostream>
#include <vector>

const float MOVE_SPEED = 100;
const float PAN_SPEED = 15;

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

    // Extract flat (XZ) forward and right vectors from the transform matrix.
    // Zero the Y component so WASD stays on the horizontal plane (Minecraft-style).
    glm::vec3 localForwardVector = glm::vec3(transform->matrix[2]);
    glm::vec3 localRightVector   = glm::vec3(transform->matrix[0]);
    localForwardVector.y = 0.0f;
    localRightVector.y   = 0.0f;
    if (glm::length(localForwardVector) > 0.0001f) localForwardVector = glm::normalize(localForwardVector);
    if (glm::length(localRightVector)   > 0.0001f) localRightVector   = glm::normalize(localRightVector);

    glm::vec3 moveVector = (localRightVector * moveInput.x) + (-localForwardVector * moveInput.z);
    moveVector *= MOVE_SPEED * deltaTime;
    // Apply elevation
    moveVector.y = elevationInput * MOVE_SPEED * deltaTime;

    transform_utils::setPosition(*transform, transform->position + moveVector);
}

// Persistent yaw and pitch state for the camera.
// Storing them separately and reconstructing the quaternion each frame ensures
// zero roll regardless of floating-point drift or export imprecision from Godot.
static float sCameraYaw   = 0.0f;
static float sCameraPitch = 0.0f;
static bool  sCameraInitialized = false;

// Extract yaw and pitch from the camera's loaded quaternion, stripping any roll.
// Camera look direction in this engine = rotation * (0, 0, -1).
// Given lookDir = (-sin(yaw)*cos(pitch), sin(pitch), -cos(yaw)*cos(pitch)):
//   pitch = arcsin(lookDir.y)
//   yaw   = atan2(-lookDir.x, -lookDir.z)
static void initCameraAngles(const Transform* transform) {
    glm::vec3 lookDir = transform->rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    sCameraPitch = std::asin(glm::clamp(lookDir.y, -1.0f, 1.0f));
    sCameraYaw   = std::atan2(-lookDir.x, -lookDir.z);
    sCameraInitialized = true;
}

static void handleCursorPanning(Transform* transform, const InputEvent& event, float deltaTime) {
    if (!sCameraInitialized)
        initCameraAngles(transform);

    // Divide by 100 cuz mousePosDelta is pixel-distance, and that's BIG
    sCameraPitch += -event.mousePosDelta.y * PAN_SPEED * deltaTime / 100;
    sCameraYaw   += -event.mousePosDelta.x * PAN_SPEED * deltaTime / 100;

    // Clamp pitch so the camera can't flip upside-down
    const float MAX_PITCH = glm::radians(89.0f);
    sCameraPitch = glm::clamp(sCameraPitch, -MAX_PITCH, MAX_PITCH);

    // Reconstruct rotation as yaw * pitch — mathematically roll-free
    glm::quat yawQuat   = glm::angleAxis(sCameraYaw,   glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat pitchQuat = glm::angleAxis(sCameraPitch, glm::vec3(1.0f, 0.0f, 0.0f));

    transform_utils::setRotation(*transform, yawQuat * pitchQuat);
}

void processCameraControl(World& world) {
    // Don't move the camera while the cursor is unlocked (user pressed Escape)
    if (!world.isCursorLocked)
        return;

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
