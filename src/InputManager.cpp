#include "InputManager.h"

#include <GLFW/glfw3.h>


void InputManager::init(GLFWwindow* window) {
    // Use raw mouse motion if supported
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // disable cursor at start
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Seed lastMousePos with the actual cursor position so the very first
    // mouse callback doesn't produce a garbage delta from (realX - (-1)).
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    lastMousePos = glm::vec2(mx, my);

    glfwSetKeyCallback(window, InputManager::_handleKey);
    glfwSetCursorPosCallback(window, InputManager::_handleMousePos);
}

void InputManager::_handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool& info = keyState[key];
    InputEvent inputEvent = {
        .key = key,
        .mods = mods
    };

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        info = true;
        inputEvent.type = InputEvent::Type::KeyPress;
    }
    else if (action == GLFW_RELEASE) {
        info = false;
        inputEvent.type = InputEvent::Type::KeyRelease;
    }

    inputEventQueue.push_back(inputEvent);
}

void InputManager::_handleMousePos(GLFWwindow *window, double xpos, double ypos) {
    glm::vec2 newMousePos = glm::vec2(xpos, ypos);

    InputEvent inputEvent = {
        .type = InputEvent::Type::MouseMove,
        .mousePos = newMousePos
    };

    inputEvent.mousePosDelta = newMousePos - lastMousePos;
    lastMousePos = newMousePos;

    inputEventQueue.push_back(inputEvent);
}

void InputManager::clearInputQueue() {
    inputEventQueue.clear();
}

void InputManager::resetMousePos(GLFWwindow* window) {
    // Re-seed lastMousePos with the current cursor position so that the next
    // mouse callback doesn't produce a large junk delta after cursor re-lock.
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    lastMousePos = glm::vec2(mx, my);
}


const bool InputManager::isKeyPressed(int key) {
    return key >= 0 && keyState[key];
}

const InputEventQueue& InputManager::getInputQueue() {
    return inputEventQueue;
}
