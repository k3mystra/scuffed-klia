#ifndef Renderer
#define Renderer

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct WindowCallbackData {
    float targetAspectRatio;
    int viewportX;
    int viewportY;
    int viewportWidth;
    int viewportHeight;
    float deltaTime;
};

GLFWwindow* setupGlfwWindow(WindowCallbackData* data);

#endif  // Renderer
