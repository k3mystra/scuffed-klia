#include "SunLight.h"

SunLight::SunLight()
    : Light(), direction(0.0f, -1.0f, 0.0f), intensity(1.0f) {
}

SunLight::SunLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    : Light(color), direction(direction), intensity(intensity) {
}
