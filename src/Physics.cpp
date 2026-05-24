#include "Physics.h"
#include <glm/ext/vector_float3.hpp>

// Based on Euler integrator
// Simplest of them all
glm::vec3 Physics::updateState(PhysicsState *state, float dt) {
    state->accel = state->forcesApplied / state->mass;

    state->velocity += state->accel * dt;
    glm::vec3 positionDelta = state->velocity * dt;

    return positionDelta;
}
