#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

#include "Render.h"
#include "Transform.h"
#include "World.h"


const unsigned int INITIAL_WINDOW_WIDTH = 640;
const unsigned int INITIAL_WINDOW_HEIGHT = 480;

int main (int argc, char *argv[]) {
    // ==== ECS Migration ====
    // Data init
    World world = loadFromFile("3DScene/LarpCombat/world.txt");

    RenderSystem renderer = RenderSystem();
    renderer.renderSystemInit(world, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    // Actual game loop
    // ==== ECS Migration ====

    float deltaTime, aggregateDeltaTime = 0;
    float lastFrameTime = 0, currentFrameTime = 0;
    float phyTimeAccumulator = 0.0;
    int frameCount = 0;

    // Main render loop
    while(!glfwWindowShouldClose(renderer.getWindowPointer()))
    {
        // Delta time calculations
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        world.deltaTime = deltaTime;

        phyTimeAccumulator += deltaTime;

        updateTransform(world);
        // skybox.render(cam->getProjectionMatrix(), cam->getInvTransform());

        // Physics shit
        // while (phyTimeAccumulator >= PHYSICS_TIMESTEP) {
        //     for (Model &model : allModels) {
        //         glm::vec3 deltaPos = Physics::updateState(&model.physicsState, PHYSICS_TIMESTEP);
        //         model.setPosition(model.getPosition() + deltaPos);
        //     }
        //
        //
        //     phyTimeAccumulator -= PHYSICS_TIMESTEP;
        // }
        renderer.render(world);

        glfwSwapBuffers(renderer.getWindowPointer());
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}
