#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

#include "Components.h"


typedef uint16_t EntityID;

struct World {
    int totalEntity = 0;
    bool isCursorLocked = true;
    float deltaTime = 0;

    GLFWwindow* window = nullptr;

    Skybox skybox;

    AmbientLight ambientLight = AmbientLight();
    SunLight sunlight = SunLight();

    std::vector<EntityData> entityDataList = {};
    std::vector<Transform> transformList = {};
    std::vector<Model> modelList = {};
    std::vector<Camera> cameraList = {};
    std::vector<Animation> animList = {};

    std::unordered_map<EntityID, size_t> entityDataIndex = {};
    std::unordered_map<EntityID, size_t> transformIndex = {};
    std::unordered_map<EntityID, size_t> modelIndex = {};
    std::unordered_map<EntityID, size_t> cameraIndex = {};
    std::unordered_map<EntityID, size_t> animIndex = {};
};

World loadFromFile(std::string filename);

void processGlobalInputEvent(World& world);
