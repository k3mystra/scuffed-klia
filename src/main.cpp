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
    transform.position = glm::vec3(0, 0.0f, 0); // y = 0
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

    // Chain animations: T1Arrival -> T1BaggageTruck -> T1Departure
    chainAnimations(world, "T1Arrival", "T1BaggageTruck");
    chainAnimations(world, "T1BaggageTruck", "T1Departure");

    // Redirect T1Departure to animate the T1Arrival plane mesh instead of the duplicate departure plane.
    // This hides the duplicate plane so only one model exists in the scene.
    redirectAnimationTarget(world, "T1Departure", "departure/T1Departure/PathFollow3D/MeshInstance3D", "Arrival/T1Arrival/PathFollow3D/MeshInstance3D");

    // Initially hide the T1Arrival plane so it starts transparent and fades in when its animation begins.
    setEntityOpacity(world, "Arrival/T1Arrival/PathFollow3D/MeshInstance3D", 0.0f);

    // Start the sequence by scheduling T1Arrival immediately at 0.0f seconds
    scheduleAnimation(world, "T1Arrival", 0.0f);
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
