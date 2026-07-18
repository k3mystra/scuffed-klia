#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

#include "InputManager.h"
#include "CameraControl.h"
#include "Render.h"
#include "World.h"
#include "Animation.h"
#include "Transform.h"


const unsigned int INITIAL_WINDOW_WIDTH = 640;
const unsigned int INITIAL_WINDOW_HEIGHT = 480;

int main (int argc, char *argv[]) {
    // ==== ECS Migration ====
    // Data init
    World world = loadFromFile("3DScene/LarpCombat/world.txt");

    RenderSystem renderer = RenderSystem(world, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    InputManager::init(world.window);
    animationSystemInit(world);

    // Preserve the original scene behaviour: exported routes begin together.
    // Schedule individual routes at other times by replacing this with calls
    // such as scheduleAnimation(world, "T1Arrival", 5.0f).
    for (const Animation& animation : world.animList)
        scheduleAnimation(world, animation.name, 0.0f);

    // Actual game loop
    // ==== ECS Migration ====

    float deltaTime = 0;
    float lastFrameTime = 0, currentFrameTime = 0;

    // Main render loop
    while(!glfwWindowShouldClose(world.window))
    {
        // Delta time calculations
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        world.deltaTime = deltaTime;
        world.elapsedTime += deltaTime;

        processGlobalInputEvent(world);
        processCameraControl(world);
        processAnimation(world);

        updateTransform(world);
        // skybox.render(cam->getProjectionMatrix(), cam->getInvTransform());

        renderer.render(world);
        InputManager::clearInputQueue();

        glfwSwapBuffers(world.window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}
