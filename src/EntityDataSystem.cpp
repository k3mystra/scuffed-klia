#include "EntityDataSystem.h"

#include <fstream>
#include <string>


void entityDataSytemInit(WorldState& worldState) {
    std::ifstream& sceneFile = worldState.sceneConfig;

    sceneFile.seekg(0);

    std::string line;
    while (std::getline(sceneFile, line)) {
        if (line[0] != '#')
            continue;

        EntityData data = EntityData();
        data.name = line.substr(2, std::string::npos);

        worldState.entityDataList.push_back(data);

        worldState.totalEntity++;
    }
}
