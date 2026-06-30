#include "RenderSystem.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <variant>

using namespace std;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);


GLFWwindow* RenderSystemInit(unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
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

    GLFWwindow* window = setupGlfwWindow(&data);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK)
        return nullptr;

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);

    return window;
}

GLFWwindow* setupGlfwWindow(WindowCallbackData* data, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
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
            initialWindowWidth, initialWindowHeight, "Larp Combat", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    
    glfwSetWindowUserPointer(window, data);
    // Resize viewport on windows resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Bunch of input stuff
    // Not gonna settle them yet
    
    // Fking terrible but it is what it is
    // glfwSetKeyCallback(window, 
    //     [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    //         scene.keyCallback(window, key, scancode, action, mods);
    //         Input::absorbKeys(window, key, scancode, action, mods);
    //     }
    // );
    // // Use raw mouse motion if supported
    // if (glfwRawMouseMotionSupported())
    //     glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(
    //     window,
    //     [](GLFWwindow* window, double xpos, double ypos) {
    //        scene.cursorPosCallback(window, xpos, ypos);
    //     }
    // );
    // glfwSetMouseButtonCallback(
    //     window,
    //     [](GLFWwindow* window, int button, int action, int mods) {
    //          scene.mouseButtonCallback(window, button, action, mods);
    //     }
    // );
    // glfwSetScrollCallback(
    //     window,
    //     [](GLFWwindow* window, double xoffset, double yoffset) {
    //         scene.scrollCallback(window, xoffset, yoffset);
    //     }
    // );

    return window;
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
