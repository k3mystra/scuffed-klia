#include "AmbientLight.h"

AmbientLight::AmbientLight() : Light() {}

AmbientLight::AmbientLight(const glm::vec3& color)
    : Light(color) {}
