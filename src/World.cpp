#include "World.h"

#include "Components.h"
#include "Model.h"
#include "Transform.h"
#include "LoadOBJ.h"
#include "InputManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdio>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <iostream>
#include <unordered_map>


const std::unordered_map<int, Animation::LoopMode> NUM_TO_LOOPMODE = {
    { 0, Animation::LoopMode::LOOP_NONE },
    { 1, Animation::LoopMode::LOOP_LINEAR },
    { 2, Animation::LoopMode::LOOP_PINGPONG }
};

const std::unordered_map<std::string, Track::Type> STR_TO_TRACK_TYPE = {
    { "POS", Track::Type::POSITION },
    { "ROT", Track::Type::ROTATION },
    { "ROT_EULER", Track::Type::ROTATION },
    { "SCL", Track::Type::SCALE }
};

static EntityData loadEntityDataComponent(const std::string& line, World& world) {
    EntityData data = EntityData();
    data.name = line.substr(2, std::string::npos);

    return data;
}

static Transform loadTransformComponent(const std::string& line, World& world) {
    Transform transform = Transform();

    std::istringstream stream(line);
    stream.seekg(2);

    // Line format: position vector, rotation (quaternion: w, x, y, z ), scale vector
    // Total: 3 + 4 + 3 = 10

    transform.position = glm::vec3(0);
    for (int i = 0; i < glm::vec3::length(); i++)
        stream >> transform.position[i];

    transform.rotation = glm::quat(0, 0, 0, 0);
    for (int i = 0; i < glm::quat::length(); i++)
        stream >> transform.rotation[i];

    transform.scale = glm::vec3(0);
    for (int i = 0; i < glm::vec3::length(); i++)
        stream >> transform.scale[i];

    transform_utils::recalcTransform(transform);

    return transform;
}

static Model loadModelComponent(const std::string& line, World& world) {
    std::string filename = line.substr(2, std::string::npos);
    Model model = loadObjFile(filename);
    model.srcPath = filename;

    model_utils::printModel(model);

    return model;
}

static Camera loadCameraComponent(const std::string& line, World& world) {
    return Camera();
}

static void convertEulerRotation(Track& track) {
    glm::vec3 eulerRotation = glm::vec3(
        track.keyframeData.end()[-1],
        track.keyframeData.end()[-2],
        track.keyframeData.end()[-3]
    );

    glm::quat q = glm::quat(eulerRotation);

    // Remove previous data
    track.keyframeData.pop_back();
    track.keyframeData.pop_back();
    track.keyframeData.pop_back();

    track.keyframeData.push_back(q.w);
    track.keyframeData.push_back(q.x);
    track.keyframeData.push_back(q.y);
    track.keyframeData.push_back(q.z);
}

static void addTracks(std::istringstream& stream, Animation& anim) {
    char trackCheck;
    stream >> trackCheck;
    if (trackCheck != 'Z')
        return;

    Track track = Track();
    stream >> track.actorEntityName;

    std::string trackTypeStr;
    stream >> trackTypeStr;
    track.type = STR_TO_TRACK_TYPE.at(trackTypeStr);

    int totalKeyframes;
    stream >> totalKeyframes;

    int keyFrameCount = 0;
    while (!stream.eof() && stream.peek() != 'Z') {
        std::string nextStr;
        stream >> nextStr;
        
        if (nextStr == "K") {
            if (trackTypeStr == "ROT_EULER" && keyFrameCount != 0)
                convertEulerRotation(track);

            float timestamp;
            stream >> timestamp;
            track.keyframeTimestamps.push_back(timestamp);

            keyFrameCount++;
            continue;
        }

        float value = std::stof(nextStr, nullptr);
        track.keyframeData.push_back(value);

        stream >> std::ws;
    }

    if (keyFrameCount != totalKeyframes)
        std::cerr << "Unexpected number of keyframes (expected: " << totalKeyframes << ", got: " << keyFrameCount << ")\n";

    anim.tracks.push_back(track);

    // Check for additional tracks
    addTracks(stream, anim);
}

static Animation loadAnimComponent(const std::string& line, World& world) {
    Animation anim = Animation();

    std::istringstream stream(line);
    stream.seekg(2);

    stream >> anim.name;
    stream >> anim.duration;

    int loopModeNum;
    stream >> loopModeNum;
    anim.loopMode = NUM_TO_LOOPMODE.at(loopModeNum);

    addTracks(stream, anim);

    return anim;
}

World loadFromFile(std::string filename) {
    World world = World();

    std::ifstream worldSetupFile(filename);

    if (worldSetupFile.fail()) {
        std::cerr << "Failed to open world setup file\n";
        exit(1);
    }

    std::string line;
    // This code works since we are reading the file top-to-bottom
    // so the latest entity ID is guaranteed to be the current entity ID
    // same thing for list of components
    while (std::getline(worldSetupFile, line)) {
        switch (line[0]) {
            case '#':
                world.entityDataList.push_back(loadEntityDataComponent(line, world));
                world.entityDataIndex.insert({ world.totalEntity - 1, world.entityDataList.size() - 1 });
                world.totalEntity++;
                break;
            case 'T':
                world.transformList.push_back(loadTransformComponent(line, world));
                world.transformIndex.insert({ world.totalEntity - 1, world.transformList.size() - 1 });
                break;
            case 'M':
                world.modelList.push_back(loadModelComponent(line, world));
                world.modelIndex.insert({ world.totalEntity - 1, world.modelList.size() - 1 });
                break;
            case 'C':
                world.cameraList.push_back(loadCameraComponent(line, world));
                world.cameraIndex.insert({ world.totalEntity - 1, world.cameraList.size() - 1 });
                break;
            case 'A':
                world.animList.push_back(loadAnimComponent(line, world));
                world.animIndex.insert({ world.totalEntity - 1, world.animList.size() - 1 });
                break;
            default:
                continue;
        };
    }

    return world;
}

static void handleKeyInput(const InputEvent& event, World& world) {
    if (event.key == GLFW_KEY_Q && event.type == InputEvent::Type::KeyPress)
        glfwSetWindowShouldClose(world.window, true);
    
    if (event.key == GLFW_KEY_ESCAPE && event.type == InputEvent::Type::KeyPress) {
        if (world.isCursorLocked)
            glfwSetInputMode(world.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(world.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        world.isCursorLocked = !world.isCursorLocked;
    }
}

void processGlobalInputEvent(World& world) {
    const InputEventQueue& inputEventQueue = InputManager::getInputQueue();

    for (const InputEvent& event: inputEventQueue) {
        switch (event.type) {
            case InputEvent::Type::KeyPress:
            case InputEvent::Type::KeyRelease:
                handleKeyInput(event, world);
                break;
            default:
                continue;
        }
    }
}
