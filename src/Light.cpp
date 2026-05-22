#include "Light.h"

Light::Light() : color(1.0f, 1.0f, 1.0f) {}

Light::Light(const glm::vec3& color)
    : color(color) {}
