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
#include "Scene.h"

using namespace std;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 360;

string readFile(const char* filePath);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
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


    // ---- Subject to Change ----
    Scene scene = Scene();
    scene.objectSetup();
    vector<MeshObject> allObjs = scene.allMeshObject;
    Camera cam = scene.camera;

    vector<MeshBufferInfo> bufferInfo = vector<MeshBufferInfo>();
    // Put all objects into GPU vertex buffer
    for (MeshObject &obj : allObjs) {
        // Vertex Array Object (VAO) to store vertex attributes layout
        // for all VBO
        glGenVertexArrays(1, &obj.bufferInfo.VAO);
        glBindVertexArray(obj.bufferInfo.VAO);

        glGenBuffers(1, &obj.bufferInfo.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, obj.bufferInfo.VBO);
        vector vertices = obj.getVertices();
        // Dynamic draw so that we can change em fast later
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        // Indices shit
        glGenBuffers(1, &obj.bufferInfo.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.bufferInfo.EBO);
        vector indices = obj.getIndices();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

        // Setup vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    
    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        // Set to wireframe before full-face rendering done
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        scene.processKeyboardInput(window);

        // Clear the buffer before next render
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);


        for (MeshObject &obj : allObjs) {
            // Pass necessary params to shader
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(obj.getTransform()));
            glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(cam.getInvTransform()));
            glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));

            glBindVertexArray(obj.bufferInfo.VAO);

            // Render here
            glDrawElements(GL_TRIANGLES, obj.getIndices().size(), GL_UNSIGNED_INT, 0);
            // Unbind VAO just in case
            glBindVertexArray(0);
        }

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
