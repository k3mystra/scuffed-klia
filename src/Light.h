#pragma once

#include <glm/glm.hpp>

class Light {
    public:
        Light();
        Light(const glm::vec3& color);

        glm::vec3 color;
};
