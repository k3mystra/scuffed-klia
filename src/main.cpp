#define GLM_ENABLE_EXPERIMENTAL
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
#include <string>
#include <glm/gtx/string_cast.hpp>

#include "MeshObject.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"

using namespace std;

struct WindowCallbackData {
    float targetAspectRatio;
    int viewportX;
    int viewportY;
    int viewportWidth;
    int viewportHeight;
};

GLFWwindow* setupGlfwWindow(WindowCallbackData* data);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Initialize scene
Scene scene = Scene();


int main (int argc, char *argv[]) {
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

    GLFWwindow* window = setupGlfwWindow(&data);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK)
        return -1;

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);

    Shader mainShader = Shader("vertex_shader.glsl", "geometry_shader.glsl", "fragment_shader.glsl");

    // ---- Subject to Change ----
    scene.objectSetup();
    vector<MeshObject> allObjs = scene.allMeshObject;
    Camera cam = scene.camera;

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

    
    float deltaTime, aggregateDeltaTime = 0;
    float lastFrameTime = 0, currentFrameTime = 0;
    int frameCount = 0;
    // Main render loop
    while(!glfwWindowShouldClose(window))
    {
        // Delta time calculations
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        frameCount++;
        aggregateDeltaTime += deltaTime;

        if (frameCount == 50) {
            cout << "FPS: " << 1.0 / (aggregateDeltaTime / 50.0) << endl;
            cout << "Delta time: " << aggregateDeltaTime / 50.0 << endl;
            frameCount = 0;
            aggregateDeltaTime = 0.0;
        }

        // Clear the buffer before next render
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Clear viewport with different color
        glEnable(GL_SCISSOR_TEST);
        glScissor(data.viewportX, data.viewportY, data.viewportWidth, data.viewportHeight);
        glClearColor(
            scene.backgroundColor.r,
            scene.backgroundColor.g,
            scene.backgroundColor.b,
            1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        mainShader.use();


        for (MeshObject &obj : allObjs) {
            // Pass vertex shader transformations
            mainShader.setMat4("model", obj.getTransform());
            mainShader.setMat4("view", cam.getInvTransform());
            mainShader.setMat4("projection", cam.getProjectionMatrix());

            // Pass material color
            mainShader.setVec3("matColor", obj.material.color);

            // Pass ambient light color
            mainShader.setVec3("ambientLightColor", scene.ambientLight.color);

            // Pass diffuse light color (sunlight only for now)
            mainShader.setVec3("sunLightColor", scene.sunLight.color * scene.sunLight.intensity);
            mainShader.setVec3("sunLightDir", scene.sunLight.direction);

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

GLFWwindow* setupGlfwWindow(WindowCallbackData* data) {
    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "GLFW init failed" << endl;
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(
            scene.initialWindowWidth, scene.initialWindowHeight, "Larp Combat", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    
    glfwSetWindowUserPointer(window, data);
    // Resize viewport on windows resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Input callbacks setup
    glfwSetKeyCallback(
        window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            scene.keyCallback(window, key, scancode, action, mods);
        }
    );
    // Use raw mouse motion if supported
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(
        window,
        [](GLFWwindow* window, double xpos, double ypos) {
           scene.cursorPosCallback(window, xpos, ypos);
        }
    );
    glfwSetMouseButtonCallback(
        window,
        [](GLFWwindow* window, int button, int action, int mods) {
             scene.mouseButtonCallback(window, button, action, mods);
        }
    );
    glfwSetScrollCallback(
        window,
        [](GLFWwindow* window, double xoffset, double yoffset) {
            scene.scrollCallback(window, xoffset, yoffset);
        }
    );

    return window;
}
