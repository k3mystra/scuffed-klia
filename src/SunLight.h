#pragma once

#include "Light.h"

class SunLight : public Light {
public:
  SunLight();
  SunLight(const glm::vec3& direction, const glm::vec3& color, float intensity);

  glm::vec3 direction;
  float intensity;
};
