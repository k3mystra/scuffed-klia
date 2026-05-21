#include "Camera.h"
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : GameObject(),
    fov(45),
    aspectRatio(16.0f/9.0f),
    nearPlane(0.5f),
    farPlane(100.0f) {
}

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : GameObject(),
    fov(fov),
    aspectRatio(aspectRatio),
    nearPlane(nearPlane),
    farPlane(farPlane) {
}

float Camera::getFov() const { return fov; }
void Camera::setFov(float newFov) { fov = newFov; }

float Camera::getAspectRatio() const { return aspectRatio; }
void Camera::setAspectRatio(float newAspectRatio) { aspectRatio = newAspectRatio; }

float Camera::getNearPlane() const { return nearPlane; }
void Camera::setNearPlane(float newNearPlane) { nearPlane = newNearPlane; }

float Camera::getFarPlane() const { return farPlane; }
void Camera::setFarPlane(float newFarPlane) { farPlane = newFarPlane; }

glm::mat4 Camera::getViewMatrix() const {
    // The view matrix is the inverse of the camera's global transform
    return getInvTransform();
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
