#pragma once

#include "Camera.h"
#include "MeshObject.h"

#include <vector>
#include <GLFW/glfw3.h>

using namespace std;

class Scene {
    public:
        Scene();
        ~Scene();

        vector<MeshObject> allMeshObject;
        Camera camera;

        void objectSetup();
        void processKeyboardInput(GLFWwindow* window);
};
