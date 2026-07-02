#pragma once

#include "World.h"

struct WindowCallbackData {
    float targetAspectRatio;
    unsigned int viewportX;
    unsigned int viewportY;
    unsigned int viewportWidth;
    unsigned int viewportHeight;
    float deltaTime;
};

GLFWwindow* RenderSystemInit(WorldState& worldState, unsigned int initialWindowWidth, unsigned int initialWindowHeight);
