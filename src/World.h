#pragma once

#include <GLFW/glfw3.h>

#include <vector>
#include <unordered_map>
#include <fstream>

#include "Components.h"
#include "InputSystem.h"


typedef uint16_t EntityID;

struct WorldState {
    GLFWwindow* window = nullptr;
    Skybox skybox;

    std::ifstream sceneConfig;

    std::vector<EntityData> entityDataList = {};
    std::vector<Transform> transformList = {};
    std::vector<Light> lightList = {};
    std::vector<Model> modelList = {};

    std::unordered_map<EntityID, size_t> lightIndex = {};
    std::unordered_map<EntityID, size_t> modelIndex = {};

    std::vector<bool> keyState = std::vector(GLFW_KEY_LAST + 1, false);
    std::vector<InputEvent> caughtInputEventList = {};
};
