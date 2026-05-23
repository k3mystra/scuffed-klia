#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath) {
    std::string vertexCode;
    std::string geometryCode;
    std::string fragmentCode;

    // Load vertex shader
    if (!vertexPath.empty()) {
        vertexCode = readFile(vertexPath);
    }
    
    // Load geometry shader
    if (!geometryPath.empty()) {
        geometryCode = readFile(geometryPath);
    }

    // Load fragment shader
    if (!fragmentPath.empty()) {
        fragmentCode = readFile(fragmentPath);
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
    ID = glCreateProgram();
    if (!vertexCode.empty() && vertex != 0) glAttachShader(ID, vertex);
    if (!geometryCode.empty() && geometry != 0) glAttachShader(ID, geometry);
    if (!fragmentCode.empty() && fragment != 0) glAttachShader(ID, fragment);
    glLinkProgram(ID);
    
    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders as they're linked into our program now and no longer necessary
    if (!vertexCode.empty() && vertex != 0) glDeleteShader(vertex);
    if (!geometryCode.empty() && geometry != 0) glDeleteShader(geometry);
    if (!fragmentCode.empty() && fragment != 0) glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

std::string Shader::readFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int shaderType, const char* source) const {
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

void Shader::setBool(const std::string &name, bool value) const {         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const { 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const { 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const { 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); 
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
