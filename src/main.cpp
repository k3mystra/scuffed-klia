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

void createGrassPlane(World& world) {
    EntityID grassID = world.totalEntity;
    world.totalEntity++;

    // Create EntityData
    EntityData data;
    data.name = "GrassPlane";
    world.entityDataList.push_back(data);
    world.entityDataIndex.insert({grassID, world.entityDataList.size() - 1});
    world.nameToIdMapping.insert({data.name, grassID});

    // Create Transform
    Transform transform;
    transform.position = glm::vec3(0, -2, 0); // y = 0
    transform.rotation = glm::quat(1, 0, 0, 0);
    transform.scale = glm::vec3(1, 1, 1);
    transform.matrix = glm::mat4(1.0f);
    world.transformList.push_back(transform);
    world.transformIndex.insert({grassID, world.transformList.size() - 1});

    // Create Model with a single quad mesh
    Model model;
    model.srcPath = "3DScene/LarpCombat/grass_texture.jpg"; // Path matches uniform comparison

    Mesh mesh;
    float halfSize = 5000.0f;
    // Layout per vertex: pos.x, pos.y, pos.z, tex.u, tex.v, norm.x, norm.y, norm.z
    mesh.vertices = {
        -halfSize, 0.0f, -halfSize,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -halfSize, 0.0f,  halfSize,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         halfSize, 0.0f,  halfSize,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         halfSize, 0.0f, -halfSize,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f
    };
    mesh.faceIndices = {
        0, 1, 2,
        2, 3, 0
    };

    mesh.material.color = glm::vec3(1.0f);
    mesh.material.diffuseTexturePath = "3DScene/LarpCombat/grass_texture.jpg";

    model.meshes.push_back(mesh);
    world.modelList.push_back(model);
    world.modelIndex.insert({grassID, world.modelList.size() - 1});
}

int main (int argc, char *argv[]) {
    // ==== ECS Migration ====
    // Data init
    World world = loadFromFile("3DScene/LarpCombat/world.txt");
    createGrassPlane(world);

    RenderSystem renderer = RenderSystem(world, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    InputManager::init(world.window);
    animationSystemInit(world);

    // Chain animations per terminal (Arrival -> BaggageTruck -> Departure)
    // Terminal 1
    chainAnimations(world, "T1Arrival", "T1BaggageTruck");
    chainAnimations(world, "T1BaggageTruck", "T1Departure");
    // Terminal 2
    chainAnimations(world, "T2Arrival", "T2BaggageTruck");
    chainAnimations(world, "T2BaggageTruck", "T2Departure");
    // Terminal 3 (Note: Departure is spelled "T3Depature" in world.txt)
    chainAnimations(world, "T3Arrival", "T3BaggageTruck");
    chainAnimations(world, "T3BaggageTruck", "T3Depature");
    // Terminal 4
    chainAnimations(world, "T4Arrival", "T4BaggageTruck");
    chainAnimations(world, "T4BaggageTruck", "T4Departure");
    // Terminal 5
    chainAnimations(world, "T5Arrival", "T5BaggageTruck");
    chainAnimations(world, "T5BaggageTruck", "T5Departure");
    // Terminal 6
    chainAnimations(world, "T6Arrival", "T6BaggageTruck");
    chainAnimations(world, "T6BaggageTruck", "T6Departure");

    // Loop Passenger Ground Traffic infinitely by chaining it to itself
    chainAnimations(world, "PassengerTruckPath", "PassengerTruckPath");

    // Redirect departure animation targets to the arrival plane models to avoid rendering duplicates
    redirectAnimationTarget(world, "T1Departure", "departure/T1Departure/PathFollow3D/MeshInstance3D", "Arrival/T1Arrival/PathFollow3D/MeshInstance3D");
    redirectAnimationTarget(world, "T2Departure", "departure/T2Departure/PathFollow3D/BigCommercial2", "Arrival/T2Arrival/PathFollow3D/MeshInstance3D");
    redirectAnimationTarget(world, "T3Depature", "departure/T3Depature/PathFollow3D/183AirplaneAirport1", "Arrival/T3Arrival/PathFollow3D/MeshInstance3D");
    redirectAnimationTarget(world, "T4Departure", "departure/T4Departure/PathFollow3D/181AirplaneAirport0", "Arrival/T4Arrival/PathFollow3D/MeshInstance3D");
    redirectAnimationTarget(world, "T5Departure", "departure/T5Departure/PathFollow3D/MeshInstance3D", "Arrival/T5Arrival/PathFollow3D/MeshInstance3D");
    redirectAnimationTarget(world, "T6Departure", "departure/T6Departure/PathFollow3D/BudgetPlane3", "Arrival/T6Arrival/PathFollow3D/MeshInstance3D");

    // Initially hide all arrival planes so they fade in when their landing sequence begins
    setEntityOpacity(world, "Arrival/T1Arrival/PathFollow3D/MeshInstance3D", 0.0f);
    setEntityOpacity(world, "Arrival/T2Arrival/PathFollow3D/MeshInstance3D", 0.0f);
    setEntityOpacity(world, "Arrival/T3Arrival/PathFollow3D/MeshInstance3D", 0.0f);
    setEntityOpacity(world, "Arrival/T4Arrival/PathFollow3D/MeshInstance3D", 0.0f);
    setEntityOpacity(world, "Arrival/T5Arrival/PathFollow3D/MeshInstance3D", 0.0f);
    setEntityOpacity(world, "Arrival/T6Arrival/PathFollow3D/MeshInstance3D", 0.0f);

    // Schedule the ground traffic and staggered terminal arrivals
    scheduleAnimation(world, "PassengerTruckPath", 0.0f);
    scheduleAnimation(world, "T1Arrival", 0.0f);
    scheduleAnimation(world, "T3Arrival", 15.0f);
    scheduleAnimation(world, "T5Arrival", 30.0f);
    scheduleAnimation(world, "T2Arrival", 45.0f);
    scheduleAnimation(world, "T4Arrival", 60.0f);
    scheduleAnimation(world, "T6Arrival", 75.0f);
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

        // Center the grass plane on the camera's XZ position so it scrolls infinitely
        if (!world.cameraList.empty()) {
            EntityID camEntityID = 0;
            for (auto& pair : world.cameraIndex) {
                camEntityID = pair.first;
                break;
            }
            auto camTransformSearch = world.transformIndex.find(camEntityID);
            if (camTransformSearch != world.transformIndex.end()) {
                glm::vec3 camPos = world.transformList[camTransformSearch->second].position;
                
                auto grassSearch = world.nameToIdMapping.find("GrassPlane");
                if (grassSearch != world.nameToIdMapping.end()) {
                    auto grassTransformSearch = world.transformIndex.find(grassSearch->second);
                    if (grassTransformSearch != world.transformIndex.end()) {
                        Transform& t = world.transformList[grassTransformSearch->second];
                        t.position.x = camPos.x;
                        t.position.z = camPos.z;
                    }
                }
            }
        }

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
