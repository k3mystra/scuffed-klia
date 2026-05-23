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

string readFile(const char* filePath);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
unsigned int compileShader(unsigned int shaderType, const char *source);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

struct WindowCallbackData {
    float targetAspectRatio;
    int viewportX;
    int viewportY;
    int viewportWidth;
    int viewportHeight;
};


int main (int argc, char *argv[]) {
    // Initialize scene
    Scene scene = Scene();

    // Initialize GLFW
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(
            scene.initialWindowWidth, scene.initialWindowHeight, "Larp Combat", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // By default already set to screen size, but useful if we resize the windows later
    glViewport(0, 0, scene.initialWindowWidth, scene.initialWindowHeight);
    // Pass WindowCallbackData for use by any callbacks
    WindowCallbackData data {
        .targetAspectRatio = scene.targetAspectRatio,
        .viewportX = 0,
        .viewportY = 0,
        .viewportWidth = scene.initialWindowWidth,
        .viewportHeight = scene.initialWindowHeight
    };
    glfwSetWindowUserPointer(window, &data);
    // Resize viewport on windows resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK)
        return -1;

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);

    // Compile shaders and link to make a 'program' to run on a GPU
    string vertexShaderString = readFile("vertex_shader.glsl");
    string geometryShaderString = readFile("geometry_shader.glsl");
    string fragmentShaderString = readFile("fragment_shader.glsl");
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderString.c_str());
    unsigned int geometryShader = compileShader(GL_GEOMETRY_SHADER, geometryShaderString.c_str());
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderString.c_str());
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, geometryShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);


    // ---- Subject to Change ----
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

        // Clear viewport with different color
        glEnable(GL_SCISSOR_TEST);
        glScissor(data.viewportX, data.viewportY, data.viewportWidth, data.viewportHeight);
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);  // Dark blue
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        glUseProgram(program);


        for (MeshObject &obj : allObjs) {
            // Pass vertex shader transformations
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(obj.getTransform()));
            glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(cam.getInvTransform()));
            glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));

            // Pass material color
            glm::vec3 matColor = obj.material.color;
            glUniform3f(glGetUniformLocation(program, "matColor"), matColor.r, matColor.g, matColor.b);

            // Pass ambient light color
            glm::vec3 ambientLightFinalColor = scene.ambientLight.color;
            glUniform3f(glGetUniformLocation(program, "ambientLightColor"), ambientLightFinalColor.r, ambientLightFinalColor.g, ambientLightFinalColor.b);

            // Pass diffuse light color (sunlight only for now)
            glm::vec3 sunlightColor = scene.sunLight.color * scene.sunLight.intensity;
            glUniform3f(glGetUniformLocation(program, "sunLightColor"), sunlightColor.r, sunlightColor.g, sunlightColor.b);
            glm::vec3 sunlightDir = scene.sunLight.direction;
            glUniform3f(glGetUniformLocation(program, "sunLightDir"), sunlightDir.x, sunlightDir.y, sunlightDir.z);

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

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowCallbackData *data = (WindowCallbackData*)glfwGetWindowUserPointer(window);
    float targetAspectRatio = data->targetAspectRatio;

    float windowAspect = (float)width / (float)height;
    
    int viewportWidth, viewportHeight;
    int viewportX = 0, viewportY = 0;
    
    if (windowAspect > targetAspectRatio) {
        // Window is wider than target - add bars on sides
        viewportHeight = height;
        viewportWidth = (int)(height * targetAspectRatio);
        viewportX = (width - viewportWidth) / 2;
    } else {
        // Window is taller than target - add bars on top/bottom
        viewportWidth = width;
        viewportHeight = (int)(width / targetAspectRatio);
        viewportY = (height - viewportHeight) / 2;
    }
    
    data->viewportX = viewportX;
    data->viewportY = viewportY;
    data->viewportWidth = viewportWidth;
    data->viewportHeight = viewportHeight;

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}
