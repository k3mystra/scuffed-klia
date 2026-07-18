#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

struct InputEvent {
    enum class Type {
        KeyPress,
        KeyRelease,
        MouseMove,
        MouseButtonPress,
        MouseButtonRelease,
        Scroll
    };

    Type type;
 
    // Keyboard
    int key = -1;          // GLFW_KEY_*
    int mods = 0;          // GLFW_MOD_*
 
    // Mouse
    int button = -1;       // GLFW_MOUSE_BUTTON_*
    glm::vec2 mousePos = glm::vec2(0);
    glm::vec2 mousePosDelta = glm::vec2(0);
    // glm::vec2 scroll = glm::vec2(0);
};

typedef std::vector<InputEvent> InputEventQueue;


class InputManager {
public:
    static void init(GLFWwindow* window);
    static void _handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _handleMousePos(GLFWwindow* window, double xpos, double ypos);
    static void clearInputQueue();
    static void resetMousePos(GLFWwindow* window);
    static const std::vector<InputEvent>& getInputQueue();
    static const bool isKeyPressed(int key);
    
private:
    static inline std::array<bool, GLFW_KEY_LAST + 1> keyState = {};
    static inline glm::vec2 lastMousePos = glm::vec2(0);
    static inline InputEventQueue inputEventQueue = {};
};
