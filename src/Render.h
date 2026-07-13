#pragma once

#include "World.h"
#include "Components.h"

#include <GLFW/glfw3.h>

struct WindowCallbackData {
    float targetAspectRatio;
    unsigned int viewportX;
    unsigned int viewportY;
    unsigned int viewportWidth;
    unsigned int viewportHeight;
    float deltaTime;
};

class RenderSystem {
public:
    RenderSystem();
    void renderSystemInit(World& world, unsigned int initialWindowWidth, unsigned int initialWindowHeight);
private:
    const Shader DEFAULT_SHADER;
    GLFWwindow* window;
    WindowCallbackData windowCallbackData;

    void initializeComponents(World& world);
    void resetBuffer();
};

