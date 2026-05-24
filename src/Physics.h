#pragma once

#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

#define PHYSICS_TIMESTEP 0.02

struct PhysicsState {
    float mass = 1;
    glm::vec3 forcesApplied = glm::vec3(0.0);
    glm::vec3 velocity = glm::vec3(0.0);
    glm::vec3 accel = glm::vec3(0.0);
};

namespace Physics {
    glm::vec3 updateState(PhysicsState* state, float dt);
};
