#pragma once

#include "GameObject.h"

class Camera : public GameObject {
public:
    Camera();
    Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

    float getFov() const;
    void setFov(float fov);

    float getAspectRatio() const;
    void setAspectRatio(float aspectRatio);

    float getNearPlane() const;
    void setNearPlane(float nearPlane);

    float getFarPlane() const;
    void setFarPlane(float farPlane);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

private:
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
};
