#pragma once

#include "World.h"


void animationSystemInit(World& world);
void processAnimation(World& world);

// Schedules a clip by its exported Path3D name. startTime is measured in
// seconds from program start; the clip remains idle until that moment.
bool scheduleAnimation(World& world, const std::string& animationName, float startTime);
