#pragma once

#include "Camera.h"
#include "MeshObject.h"
#include "SunLight.h"
#include "AmbientLight.h"
#include "Model.h"

#include <vector>
#include <GLFW/glfw3.h>

using namespace std;

struct SceneObject {
    Model model;
    glm::vec3 position;
    glm::vec3 rotation; // degrees
    glm::vec3 scale;
};

class Scene {
    public:
        Scene();
        ~Scene();

        int initialWindowWidth;
        int initialWindowHeight;
        float targetAspectRatio;

        glm::vec3 backgroundColor;

        vector<Model> allModels;
        Camera camera;

        SunLight sunLight;
        AmbientLight ambientLight;

        bool isCursorLocked;
        bool isDragging;
        float dragSpeedMultiplier;
        glm::vec2 lastCursorPos;
        bool isKeyHold[GLFW_KEY_LAST + 1];

        void objectSetup();
        void process(float deltaTime);
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
