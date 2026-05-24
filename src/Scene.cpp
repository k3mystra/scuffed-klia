#include "Input.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Scene.h"
#include "Camera.h"
#include "MeshObject.h"
#include "SunLight.h"
#include "WindowCallbackData.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cerrno>
#include <cstring>

#define check_bit(mods,bit) (mods & bit) == bit


Scene::Scene() {
    initialWindowWidth = 1280;
    initialWindowHeight = 720;
    targetAspectRatio = 16.0 / 9.0f;

    isCursorLocked = true;

    allMeshObject = vector<MeshObject>();
    camera = Camera();
    // Kinda orange, going down
    sunLight = SunLight(
        glm::vec3(-0.1961, -0.7845, 0.5883),
        glm::vec3(1.0),
        1.2
    );
    ambientLight = AmbientLight(glm::vec3(1.0, 1.0, 1.0));
    backgroundColor = glm::vec3(0.1);
}

Scene::~Scene() {
}

bool isPressOrRelease(int act) {
    return act == GLFW_PRESS || act == GLFW_REPEAT;
}

// Reminder that this will be called PER KEYPRESS
// Only 1 keypress will be detected at a time
void Scene::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowCallbackData *data = (WindowCallbackData*)glfwGetWindowUserPointer(window);

    // Bypass press/repeat key model with press -> hold -> release model
    if (action == GLFW_PRESS)
        isKeyHold[key] = true;
    else if (action == GLFW_RELEASE)
        isKeyHold[key] = false;

    // Ctrl + Q
    if (key == GLFW_KEY_Q && check_bit(mods, GLFW_MOD_CONTROL) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (isCursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isCursorLocked = false;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isCursorLocked = true;
        }
    }
}

void Scene::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {}

void Scene::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}

void Scene::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}

void Scene::process(float deltaTime) {
    const KeyInfo* wKey = Input::getKey(GLFW_KEY_W);
    const KeyInfo* aKey = Input::getKey(GLFW_KEY_A);
    const KeyInfo* sKey = Input::getKey(GLFW_KEY_S);
    const KeyInfo* dKey = Input::getKey(GLFW_KEY_D);

    glm::vec3 moveInput = glm::vec3(0.0);
    if (wKey != nullptr && wKey->isPressed) {
        moveInput.y = 1;
    }
    else if (sKey != nullptr && sKey->isPressed) {
        moveInput.y = -1;
    }

    if (aKey != nullptr && aKey->isPressed) {
        moveInput.x = -1;
    }
    else if (dKey != nullptr && dKey->isPressed) {
        moveInput.x = 1;
    }

    const float cameraMoveSpeed = 10 * deltaTime;
    glm::vec3 camPos = camera.getPosition();
    camera.setPosition(camPos + (moveInput * cameraMoveSpeed));
}

void loadObjToMeshObject(const std::string& path, MeshObject& obj) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << path << "\n";
        return;
    }

    std::string line;
    std::vector<float> rawVertices;
    std::vector<unsigned int> faceIndices;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            rawVertices.push_back(x); rawVertices.push_back(y); rawVertices.push_back(z);
        } 
        else if (prefix == "f") {
            std::string vertexData;
            while (ss >> vertexData) {
                // Obj format is v/vt/vn, we only want the 'v' part (index)
                size_t slashPos = vertexData.find('/');
                int vIdx = std::stoi(vertexData.substr(0, slashPos)) - 1; // -1 because OBJ is 1-indexed
                faceIndices.push_back(vIdx);
            }
        }
    }
    
    // Now you have your vectors ready for glBufferData!
    obj.setVertices(rawVertices);
    obj.setIndices(faceIndices);
}

std::vector<SceneObject> loadSceneLayout(const std::string& layoutPath) {
    std::vector<SceneObject> sceneObjects;
    std::ifstream file(layoutPath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open layout file: " << layoutPath << "\n";
        std::cerr << "Reason: " << std::strerror(errno) << "\n";
        return sceneObjects;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string objPath;
        float px, py, pz, rx, ry, rz, sx, sy, sz;

        ss >> objPath >> px >> py >> pz >> rx >> ry >> rz >> sx >> sy >> sz;

        if (ss.fail()) {
            std::cerr << "Warning: Skipping malformed line: " << line << "\n";
            continue;
        }

        //getting the transform data
        SceneObject sceneObj;
        sceneObj.position = glm::vec3(px, py, pz);
        sceneObj.rotation = glm::vec3(rx, ry, rz);
        sceneObj.scale    = glm::vec3(sx, sy, sz);

        //getting the mesh
        loadObjToMeshObject(objPath, sceneObj.mesh);
        sceneObjects.push_back(sceneObj);
        
        std::cout << "Loaded: " << objPath << " at (" << px << ", " << py << ", " << pz << ")\n";
    }

    return sceneObjects;
}

void Scene::objectSetup() {
    std::filesystem::path exePath = std::filesystem::current_path();
    std::cout << "Working directory: " << exePath << "\n";

    std::string layoutPath = (exePath / "3DScene/LarpCombat/scene_layout.txt").string();
    std::cout << "Final path: " << layoutPath << "\n";
    
    auto sceneObjects = loadSceneLayout(layoutPath);

    //applying the transform data to all mesh objects
    for (auto& sceneObj : sceneObjects) {
        sceneObj.mesh.setPosition(sceneObj.position);
        sceneObj.mesh.setRotation(sceneObj.rotation);
        sceneObj.mesh.setScale(sceneObj.scale);
        sceneObj.mesh.recalcTransform();

        sceneObj.mesh.material = Material { .color = glm::vec3(0.09, 0.757, 1)};
        allMeshObject.push_back(sceneObj.mesh);
    }
}
