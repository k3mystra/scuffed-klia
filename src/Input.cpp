#include "Input.h"

// Static member initialization
std::unordered_map<int, KeyInfo> Input::keys;

void Input::absorbKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto& info = keys[key];
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        info.isPressed = true;
    }
    else if (action == GLFW_RELEASE) {
        info.isPressed = false;
    }
    
    info.mods = mods;
}

void Input::refresh() {
    for (auto& [key, info] : keys) {
        info.wasPressed = info.isPressed;
    }
}

const KeyInfo* Input::getKey(int key) {
    auto it = keys.find(key);
    return (it != keys.end()) ? &it->second : nullptr;
}
