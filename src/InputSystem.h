#pragma once

#include <GLFW/glfw3.h>

#include <unordered_map>
#include <functional>
#include <variant>

using namespace std;


struct KeyInfo {
    bool isPressed = false;
    bool wasPressed = false;
    int mods = 0;
};

struct InputEvent {
    enum class Type {
        Keyboard, MouseMotion, MouseButton, MouseScroll
    };
};

class Input {
public:
    static void absorbKeys(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void refresh();
    static const KeyInfo* getKey(int key);
    
private:
    static std::unordered_map<int, KeyInfo> keys;
};
