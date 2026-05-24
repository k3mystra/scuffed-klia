#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>


struct KeyInfo {
    bool isPressed = false;
    bool wasPressed = false;
    int mods = 0;
};

class Input {
public:
    static void absorbKeys(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void refresh();
    static const KeyInfo* getKey(int key);
    
private:
    static std::unordered_map<int, KeyInfo> keys;
};
