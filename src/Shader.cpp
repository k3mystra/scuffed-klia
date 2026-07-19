#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>


static std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static unsigned int compileShader(unsigned int shaderType, const char* source) {
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string typeStr;
        if(shaderType == GL_VERTEX_SHADER) typeStr = "VERTEX";
        else if(shaderType == GL_GEOMETRY_SHADER) typeStr = "GEOMETRY";
        else if(shaderType == GL_FRAGMENT_SHADER) typeStr = "FRAGMENT";

        std::cerr << "ERROR::SHADER::" << typeStr << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

namespace shader_utils {
    void setBool(const Shader& shader, const char* name, bool value) {         
        glUniform1i(glGetUniformLocation(shader.programID, name), (int)value); 
    }
    void setInt(const Shader& shader, const char* name, int value) { 
        glUniform1i(glGetUniformLocation(shader.programID, name), value); 
    }
    void setFloat(const Shader& shader, const char* name, float value) { 
        glUniform1f(glGetUniformLocation(shader.programID, name), value); 
    }
    void setVec3(const Shader& shader, const char* name, const glm::vec3 &value) { 
        glUniform3fv(glGetUniformLocation(shader.programID, name), 1, glm::value_ptr(value)); 
    }
    void setMat4(const Shader& shader, const char* name, const glm::mat4 &value) {
        glUniformMatrix4fv(glGetUniformLocation(shader.programID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void initializeShader(Shader& shader) {
        std::string vertexCode;
        std::string geometryCode;
        std::string fragmentCode;

        // Load vertex shader
        if (!shader.vertexShaderSrcPath.empty()) {
            vertexCode = readFile(shader.vertexShaderSrcPath);
        }
        
        // Load geometry shader
        if (!shader.geometryShaderSrcPath.empty()) {
            geometryCode = readFile(shader.geometryShaderSrcPath);
        }

        // Load fragment shader
        if (!shader.fragmentShaderSrcPath.empty()) {
            fragmentCode = readFile(shader.fragmentShaderSrcPath);
        }

        // Compile shaders
        unsigned int vertex = 0, geometry = 0, fragment = 0;
        
        if (!vertexCode.empty())
            vertex = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
            
        if (!geometryCode.empty())
            geometry = compileShader(GL_GEOMETRY_SHADER, geometryCode.c_str());
            
        if (!fragmentCode.empty())
            fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

        // Shader Program
        shader.programID = glCreateProgram();
        if (!vertexCode.empty() && vertex != 0) glAttachShader(shader.programID, vertex);
        if (!geometryCode.empty() && geometry != 0) glAttachShader(shader.programID, geometry);
        if (!fragmentCode.empty() && fragment != 0) glAttachShader(shader.programID, fragment);
        glLinkProgram(shader.programID);
        
        // Check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(shader.programID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader.programID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete shaders as they're linked into our program now and no longer necessary
        if (!vertexCode.empty() && vertex != 0) glDeleteShader(vertex);
        if (!geometryCode.empty() && geometry != 0) glDeleteShader(geometry);
        if (!fragmentCode.empty() && fragment != 0) glDeleteShader(fragment);
    }
}
