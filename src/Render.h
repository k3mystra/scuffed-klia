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
    RenderSystem(World& world, unsigned int initialWindowWidth, unsigned int initialWindowHeight);
    void render(World& world);
private:
    WindowCallbackData windowCallbackData;
    Shader _defaultShader = Shader();

    Shader getDefaultShader();
    void initializeComponents(World& world);
    void resetBuffer();
};

