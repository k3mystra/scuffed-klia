#include <cstddef>
#include <fstream>
#include <filesystem>
#include <functional>
#include <iostream>
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "Render.h"
#include "World.h"
#include "Components.h"


static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
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

GLFWwindow* setupGlfwWindow(WindowCallbackData* data, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(
            initialWindowWidth, initialWindowHeight, "Larp Combat", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    
    glfwSetWindowUserPointer(window, data);
    // Resize viewport on windows resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    return window;
}

void renderSystemInit(World& world, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
     // By default already set to screen size, but useful if we resize the windows later
    glViewport(0, 0, initialWindowWidth, initialWindowHeight);
    // Pass WindowCallbackData for use by any callbacks
    WindowCallbackData data {
        .targetAspectRatio = 16.0 / 9.0,
        .viewportX = 0,
        .viewportY = 0,
        .viewportWidth = initialWindowWidth,
        .viewportHeight = initialWindowHeight,
        .deltaTime = 0
    };

    world.window = setupGlfwWindow(&data, initialWindowWidth, initialWindowHeight);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        exit(1);
    }

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);
}
