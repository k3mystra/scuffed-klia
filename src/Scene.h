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

        int initialWindowWidth = 640;
        int initialWindowHeight = 360;
        float targetAspectRatio = 16.0 / 9.0;

        glm::vec3 backgroundColor = glm::vec3(0.3);

        vector<MeshObject> allMeshObject;
        Camera camera;

        SunLight sunLight;
        AmbientLight ambientLight;

        void objectSetup();
        void processKeyboardInput(GLFWwindow* window);
};
