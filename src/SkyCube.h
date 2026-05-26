#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"

class SkyCube {
public:
    SkyCube();
    ~SkyCube();

    void init(const std::vector<std::string>& faces);
    void render(const glm::mat4& projection, const glm::mat4& view);

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int cubemapTexture;
    Shader* shader;
};
