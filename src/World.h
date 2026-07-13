#pragma once

#include <vector>
#include <unordered_map>

#include "Components.h"
#include "InputSystem.h"


typedef uint16_t EntityID;

struct World {
    int totalEntity = 0;
    float deltaTime = 0;

    Skybox skybox;

    AmbientLight ambientLight = AmbientLight();
    SunLight sunlight = SunLight();

    std::vector<EntityData> entityDataList = {};
    std::vector<Transform> transformList = {};
    std::vector<Model> modelList = {};
    std::vector<Camera> cameraList = {};

    std::unordered_map<EntityID, size_t> transformIndex = {};
    std::unordered_map<EntityID, size_t> modelIndex = {};
    std::unordered_map<EntityID, size_t> cameraIndex = {};

    std::vector<bool> keyState = std::vector(GLFW_KEY_LAST + 1, false);
    std::vector<InputEvent> caughtInputEventList = {};
};

World loadFromFile(std::string filename);
