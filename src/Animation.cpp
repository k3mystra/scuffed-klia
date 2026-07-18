#include "Animation.h"

#include "Components.h"
#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>


void animationSystemInit(World& world) {
    for (Animation& anim : world.animList) {
        for (Track& track : anim.tracks) {
            auto idSearch = world.nameToIdMapping.find(track.actorEntityName);
            if (idSearch == world.nameToIdMapping.end()) {
                std::cerr << "Animation '" << anim.name << "' target was not exported: "
                          << track.actorEntityName << "\n";
                continue;
            }

            track.cachedEntityID = idSearch->second;
        }
    }
}

bool scheduleAnimation(World& world, const std::string& animationName, float startTime) {
    auto animationIt = std::find_if(world.animList.begin(), world.animList.end(),
        [&animationName](const Animation& animation) { return animation.name == animationName; });
    if (animationIt == world.animList.end()) {
        std::cerr << "Animation not found: " << animationName << "\n";
        return false;
    }

    Animation& animation = *animationIt;
    animation.startTime = startTime;
    animation.currentTime = 0.0f;
    animation.isScheduled = true;
    animation.isPlaying = false;
    for (Track& track : animation.tracks) {
        track.currentKeyframeIdx = 0;
        track.nextKeyframeIdx = track.keyframeTimestamps.size() > 1 ? 1 : 0;
    }
    return true;
}

static glm::vec3 lerpKeyframeDataVec3(const Track& track, float currentTime) {
    // Get necessary keyframe data
    // Need to interpolate between nextKeyframe and the one just before it
    glm::vec3 initial = glm::vec3(
        track.keyframeData[(track.nextKeyframeIdx - 1) * 3 + 0],
        track.keyframeData[(track.nextKeyframeIdx - 1) * 3 + 1],
        track.keyframeData[(track.nextKeyframeIdx - 1) * 3 + 2]
    );

    glm::vec3 final = glm::vec3(
        track.keyframeData[track.nextKeyframeIdx * 3 + 0],
        track.keyframeData[track.nextKeyframeIdx * 3 + 1],
        track.keyframeData[track.nextKeyframeIdx * 3 + 2]
    );

    float nextTimestamp = track.keyframeTimestamps[track.nextKeyframeIdx];
    float prevTimestamp = track.keyframeTimestamps[track.nextKeyframeIdx - 1];
    float value = (currentTime - prevTimestamp) / (nextTimestamp - prevTimestamp);

    return glm::lerp(initial, final, value);
}

static glm::quat lerpKeyframeDataQuaternion(const Track& track, float currentTime) {
    // Get necessary keyframe data
    // Need to interpolate between nextKeyframe and the one just before it
    glm::quat initial = glm::quat(
        track.keyframeData[(track.nextKeyframeIdx - 1) * 4 + 0],
        track.keyframeData[(track.nextKeyframeIdx - 1) * 4 + 1],
        track.keyframeData[(track.nextKeyframeIdx - 1) * 4 + 2],
        track.keyframeData[(track.nextKeyframeIdx - 1) * 4 + 3]
    );

    glm::quat final = glm::quat(
        track.keyframeData[track.nextKeyframeIdx * 4 + 0],
        track.keyframeData[track.nextKeyframeIdx * 4 + 1],
        track.keyframeData[track.nextKeyframeIdx * 4 + 2],
        track.keyframeData[track.nextKeyframeIdx * 4 + 3]
    );

    float nextTimestamp = track.keyframeTimestamps[track.nextKeyframeIdx];
    float prevTimestamp = track.keyframeTimestamps[track.nextKeyframeIdx - 1];
    float value = (currentTime - prevTimestamp) / (nextTimestamp - prevTimestamp);

    return glm::slerp(initial, final, value);
}

static void processPositionChange(const Track& track, float currentTime, World& world) {
    auto transformSearch = world.transformIndex.find(track.cachedEntityID);
    if (transformSearch == world.transformIndex.end())
        return;

    Transform& transform = world.transformList[transformSearch->second];
    glm::vec3 newPos = lerpKeyframeDataVec3(track, currentTime);
    transform_utils::setPosition(transform, newPos);
}

static void processRotationChange(const Track& track, float currentTime, World& world) {
    auto transformSearch = world.transformIndex.find(track.cachedEntityID);
    if (transformSearch == world.transformIndex.end())
        return;

    Transform& transform = world.transformList[transformSearch->second];
    glm::quat newRotation = lerpKeyframeDataQuaternion(track, currentTime);
    transform_utils::setRotation(transform, newRotation);
}

static void processScaleChange(const Track& track, float currentTime, World& world) {
    auto transformSearch = world.transformIndex.find(track.cachedEntityID);
    if (transformSearch == world.transformIndex.end())
        return;

    Transform& transform = world.transformList[transformSearch->second];
    glm::vec3 newScale = lerpKeyframeDataVec3(track, currentTime);

    transform_utils::setScale(transform, newScale);
}

static size_t searchForNextKeyframe(float currentTime, const std::vector<float>& timestamps) {
    for (size_t i = 0; i < timestamps.size(); i++) {
        if (timestamps[i] > currentTime)
            return i;
    }

    // If not found, return the last keyframe
    return timestamps.size() - 1;
}

static void processTrack(Track& track, float currentTime, World& world) {
    // Recompute every frame so a loop or ping-pong clip can safely move time
    // backwards without retaining the previous cycle's keyframe index.
    track.nextKeyframeIdx = searchForNextKeyframe(currentTime, track.keyframeTimestamps);
    if (track.nextKeyframeIdx == 0)
        track.nextKeyframeIdx = 1;

    switch (track.type) {
        case Track::Type::POSITION:
            processPositionChange(track, currentTime, world);
            break;
        case Track::Type::ROTATION:
            processRotationChange(track, currentTime, world);
            break;
        case Track::Type::SCALE:
            processScaleChange(track, currentTime, world);
            break;
        break;
    }
}

void processAnimation(World& world) {
    for (Animation& anim : world.animList) {
        if (!anim.isScheduled || world.elapsedTime < anim.startTime)
            continue;

        anim.isPlaying = true;
        anim.currentTime = world.elapsedTime - anim.startTime;
        if (anim.duration <= 0.0f)
            continue;

        if (anim.currentTime >= anim.duration) {
            if (anim.loopMode == Animation::LoopMode::LOOP_NONE) {
                anim.isPlaying = false;
                continue;
            }
            if (anim.loopMode == Animation::LoopMode::LOOP_LINEAR) {
                anim.currentTime = std::fmod(anim.currentTime, anim.duration);
            } else {
                const float cycleTime = std::fmod(anim.currentTime, anim.duration * 2.0f);
                anim.currentTime = cycleTime <= anim.duration ? cycleTime : anim.duration * 2.0f - cycleTime;
            }
        }

        for (Track& track : anim.tracks) {
			if (track.keyframeTimestamps.size() < 2)
				continue;
            processTrack(track, anim.currentTime, world);
        }
    }
}
