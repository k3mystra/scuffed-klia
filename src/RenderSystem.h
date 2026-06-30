#ifndef RenderSystem
#define RenderSystem

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct WindowCallbackData {
    float targetAspectRatio;
    unsigned int viewportX;
    unsigned int viewportY;
    unsigned int viewportWidth;
    unsigned int viewportHeight;
    float deltaTime;
};

GLFWwindow* RenderSystemInit(unsigned int initialWindowWidth, unsigned int initialWindowHeight);
GLFWwindow* setupGlfwWindow(WindowCallbackData* data);

#endif  // Render
