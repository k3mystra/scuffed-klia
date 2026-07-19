#pragma once

#include "World.h"


void animationSystemInit(World& world);
void processAnimation(World& world);

// Schedules a clip by its exported Path3D name. startTime is measured in
// seconds from program start; the clip remains idle until that moment.
bool scheduleAnimation(World& world, const std::string& animationName, float startTime);

// Chains two animations together: when "first" finishes playing, "second" will be scheduled immediately.
void chainAnimations(World& world, const std::string& first, const std::string& second);

// Redirects all tracks in animationName targeting oldTargetName to target newTargetName instead.
// Also hides the old target entity's model from rendering.
void redirectAnimationTarget(World& world, const std::string& animationName, const std::string& oldTargetName, const std::string& newTargetName);

// Set initial opacity of a named entity.
void setEntityOpacity(World& world, const std::string& entityName, float opacity);
