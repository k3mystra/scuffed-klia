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

        vector<MeshObject> allMeshObject;
        Camera camera;
        SunLight sunLight;
        AmbientLight ambientLight;

        void objectSetup();
        void processKeyboardInput(GLFWwindow* window);
};
