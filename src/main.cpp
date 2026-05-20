#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

#include "GameObject.h"

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 700;

const char *vertexShaderSource =
"#version 330 core\n"
// Get vertex attributes (for now, just the position)
// location 0 means we want to pick the 0th vertex attribute
// as defined in a glVertexAttribPointer() call
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
// Output of vertex shader is gl_Position
" gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 objectColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(objectColor, 1.0);\n"
"}\n\0";


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int compileShader(unsigned int shaderType, const char *source);


int main (int argc, char *argv[]) {
    // Initialize GLFW
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(
            WINDOW_WIDTH, WINDOW_HEIGHT, "Larp Combat", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // By default already set to screen size, but useful if we resize the windows later
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Resize viewport on windows resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK)
        return -1;

    // Compile shaders and link to make a 'program' to run on a GPU
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // Tell OpenGL to use the 'program' for the every rendering call
    glUseProgram(program);
    

    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);


        // Clear the buffer before next render
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);


        // Render here
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}

unsigned int compileShader(unsigned int shaderType, const char *source) {
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cerr << "Shader compilation failed: " << infoLog << std::endl;
        return 0;
    }

    return shader;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Equivalent of _input() in godot
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
