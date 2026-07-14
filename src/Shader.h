#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components.h"


namespace shader_utils {
    void initializeShader(Shader& shader);

    void setBool(const Shader& shader, const std::string &name, bool value);         
    void setInt(const Shader& shader, const std::string &name, int value); 
    void setFloat(const Shader& shader, const std::string &name, float value); 
    void setVec3(const Shader& shader, const std::string &name, const glm::vec3 &value); 
    void setMat4(const Shader& shader, const std::string &name, const glm::mat4 &value);
};

