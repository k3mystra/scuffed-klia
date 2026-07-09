#include "World.h"

#include "Transform.h"
#include "LoadOBJ.h"

#include <sstream>
#include <string>
#include <iostream>


static void loadEntityDataComponent(const std::string& line, World& world) {
    EntityData data = EntityData();
    data.name = line.substr(2, std::string::npos);

    world.entityDataList.push_back(data);
}

static void loadTransformComponent(const std::string& line, World& world) {
    Transform transform = Transform();
    transform.isDirty = false;
    transform.matrix = glm::mat4(1);

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

    world.transformList.push_back(transform);
}

static void loadRenderComponent(const std::string& line, World& world) {
    std::string filename = line.substr(2, std::string::npos);
    Model model = loadObjFile(filename);
    model.srcPath = filename;

    world.modelList.push_back(model);
}

World loadFromFile(std::string filename) {
    World world = World();

    std::ifstream worldSetupFile(filename);

    std::string line;
    // This code works since we are reading the file top-to-bottom
    // so the latest entity ID is guaranteed to be the current entity ID
    // same thing for list of components
    while (std::getline(worldSetupFile, line)) {
        switch (line[0]) {
            case '#':
                loadEntityDataComponent(line, world);
                world.totalEntity++;
                break;
            case 'T':
                loadTransformComponent(line, world);
                world.transformIndex.insert({ world.totalEntity - 1, world.transformList.size() - 1 });
                break;
            case 'R':
                loadRenderComponent(line, world);
                world.modelIndex.insert({ world.totalEntity - 1, world.modelList.size() - 1 });
                break;
            default:
                continue;
        };
    }

    return world;
}
