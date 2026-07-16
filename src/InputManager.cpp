#include "InputManager.h"

#include <GLFW/glfw3.h>


// Static member initialization
std::array<bool, GLFW_KEY_LAST + 1> InputManager::keyState = {};
glm::vec2 lastMousePos = glm::vec2(-1, -1);
std::vector<InputEvent> inputEventQueue = {};

void InputManager::init(GLFWwindow* window) {
    // Use raw mouse motion if supported
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // disable cursor at start
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    if (lastMousePos == glm::vec2(-1, -1)) {
        lastMousePos = newMousePos;
    }

    inputEvent.mousePosDelta = newMousePos - lastMousePos;
    inputEventQueue.push_back(inputEvent);
}

void InputManager::clearInputQueue() {
    inputEventQueue.clear();
}

const bool InputManager::isKeyPressed(int key) {
    return key >= 0 && keyState[key];
}

const std::vector<InputEvent>& InputManager::getInputQueue() {
    return inputEventQueue;
}
