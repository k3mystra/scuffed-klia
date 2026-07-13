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

    std::vector<EntityData> entityDataList = {};
    std::vector<Transform> transformList = {};
    std::vector<Light> lightList = {};
    std::vector<Model> modelList = {};

    std::unordered_map<EntityID, size_t> transformIndex = {};
    std::unordered_map<EntityID, size_t> lightIndex = {};
    std::unordered_map<EntityID, size_t> modelIndex = {};

    std::vector<bool> keyState = std::vector(GLFW_KEY_LAST + 1, false);
    std::vector<InputEvent> caughtInputEventList = {};
};

World loadFromFile(std::string filename);
