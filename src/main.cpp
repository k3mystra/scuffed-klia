#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "MeshObject.h"
#include "Camera.h"

using namespace std;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 360;

string readFile(const char* filePath);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int compileShader(unsigned int shaderType, const char *source);
MeshObject generate3dObject();


int main (int argc, char *argv[]) {
    // Initialize GLFW
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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
    // glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK)
        return -1;

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);

    // Compile shaders and link to make a 'program' to run on a GPU
    string vertexShaderString = readFile("vertex_shader.glsl");
    string fragmentShaderString = readFile("fragment_shader.glsl");
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderString.c_str());
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderString.c_str());
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex Array Object (VAO) to store vertex attributes layout
    // for all VBO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // ---- Subject to Change ----
    MeshObject obj = generate3dObject();
    obj.setPosition(glm::vec3(0.0, 0.0, -3.0));
    obj.setRotation(glm::vec3(0.0, 20.0, 10.0));
    // obj.setScale(glm::vec3(0.5));
    obj.recalcTransform();

    Camera cam = Camera();

    // Put all objects into GPU vertex buffer
    // Since only 1, we don't use for loop here
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    vector vertices = obj.getVertices();
    // Dynamic draw so that we can change em fast later
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Indices shit
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    vector indices = obj.getIndices();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    
    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        // Set to wireframe before full-face rendering done
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        processInput(window);

        // Clear the buffer before next render
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // Pass necessary params to shader
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(obj.getTransform()));
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(cam.getInvTransform()));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));

        glBindVertexArray(VAO);
        // Render here, all 36 indices of a cube
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // Unbind VAO just in case
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}

string readFile(const char* filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filePath << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
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

MeshObject generate3dObject() {
  // For now just cube
  vector<float> vertices = {0.5, 0.5,  0.5,  -0.5, 0.5,  0.5, -0.5, -0.5,
                            0.5, 0.5,  -0.5, 0.5,  0.5,  0.5, -0.5, -0.5,
                            0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5};

  // Index start with 0 here
  vector<unsigned int> indices = {0, 1, 2, 0, 2, 3, 0, 4, 5, 0, 5, 1, 2, 6, 7, 2, 7, 3, 7, 6, 5, 7, 5, 4, 1, 5, 6, 1, 6, 2, 3, 7, 4, 3, 4, 0};

  MeshObject obj = MeshObject();
  obj.setVertices(vertices);
  obj.setIndices(indices);

  return obj;
}
