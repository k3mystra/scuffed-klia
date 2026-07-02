#pragma once

#include <GLFW/glfw3.h>

#include <vector>
#include <unordered_map>

#include "Components.h"
#include "InputSystem.h"


typedef uint16_t EntityID;

struct WorldState {
    std::vector<Transform> transformList = {};
    std::vector<Light> lightList = {};
    std::vector<Model> modelList = {};

    std::unordered_map<EntityID, size_t> transformIndex = {};
    std::unordered_map<EntityID, size_t> lightIndex = {};
    std::unordered_map<EntityID, size_t> modelIndex = {};

    vector<bool> keyState = vector(GLFW_KEY_LAST + 1, false);

    Skybox skybox;

    vector<InputEvent> caughtInputEventList = {};
};
