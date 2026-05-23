#pragma once

#include "Camera.h"
#include "MeshObject.h"
#include "SunLight.h"
#include "AmbientLight.h"

#include <vector>
#include <GLFW/glfw3.h>

using namespace std;

class Scene {
    public:
        Scene();
        ~Scene();

        int initialWindowWidth;
        int initialWindowHeight;
        float targetAspectRatio;

        glm::vec3 backgroundColor;

        vector<MeshObject> allMeshObject;
        Camera camera;

        SunLight sunLight;
        AmbientLight ambientLight;

        bool isCursorLocked;

        void objectSetup();
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
