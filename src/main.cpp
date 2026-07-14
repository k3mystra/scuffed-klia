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

// New plan
// ECS system cuz why not
// Data-oriented
// Entity are just IDs
// Components are just struct of data
// System are functions that processes those components data
//
// Components (so far): Mesh, Light, Skybox, Transform

int main (int argc, char *argv[]) {
    // ==== ECS Migration ====
    // Data init
    World world = loadFromFile("world.txt");

    // New scene file format:
    // # Entity Name
    // The rest of the info
    // <space>
    // ...repeat

    RenderSystem renderer = RenderSystem();
    renderer.renderSystemInit(world, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    // Actual game loop
    // ==== ECS Migration ====

    // ---- Subject to Change ----
    // scene.objectSetup();
    //
    // // Put all objects into GPU vertex buffer
    //
    //     // Sea plane setup
    // glGenVertexArrays(1, &scene.sea.bufferInfo.VAO);
    // glBindVertexArray(scene.sea.bufferInfo.VAO);
    //
    // glGenBuffers(1, &scene.sea.bufferInfo.VBO);
    // glBindBuffer(GL_ARRAY_BUFFER, scene.sea.bufferInfo.VBO);
    // auto seaVerts = scene.sea.getVertices();
    // glBufferData(GL_ARRAY_BUFFER, seaVerts.size() * sizeof(float), seaVerts.data(), GL_STATIC_DRAW);
    //
    // glGenBuffers(1, &scene.sea.bufferInfo.EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene.sea.bufferInfo.EBO);
    // auto seaIdx = scene.sea.getIndices();
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, seaIdx.size() * sizeof(unsigned int), seaIdx.data(), GL_STATIC_DRAW);
    //
    // // Same layout as models: pos(3) + uv(2) + normal(3)
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    // glEnableVertexAttribArray(2);
    //
    // Load textures
    // scene.sea.diffuseTextureID = loadTexture("3DScene/LarpCombat/water.jpg");
    // scene.sea.normalTextureID  = loadTexture("3DScene/LarpCombat/water_normal.jpg");

    //
    // SkyCube skybox;
    // std::vector<std::string> skyboxFaces = {
    //     "3DScene/LarpCombat/skybox_right.png",
    //     "3DScene/LarpCombat/skybox_left.png",
    //     "3DScene/LarpCombat/skybox_top.png",
    //     "3DScene/LarpCombat/skybox_bottom.png",
    //     "3DScene/LarpCombat/skybox_front.png",
    //     "3DScene/LarpCombat/skybox_back.png"
    // };
    // skybox.init(skyboxFaces);
    //

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

        // frameCount++;
        // aggregateDeltaTime += deltaTime;

        // if (frameCount == 50) {
        //     cout << "FPS: " << 1.0 / (aggregateDeltaTime / 50.0) << endl;
        //     cout << "Delta time: " << aggregateDeltaTime / 50.0 << endl;
        //     frameCount = 0;
        //     aggregateDeltaTime = 0.0;
        // }

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

        // Snap sea to camera X/Z so it appears infinite
        //-----------------------------------------------------------------------------------------
        // glm::vec3 camPos = scene.camera.getPosition();
        // scene.sea.setPosition(glm::vec3(camPos.x, 0.0f, camPos.z));
        // scene.sea.recalcTransform();
        // mainShader.setMat4("model", scene.sea.getTransform());
        // mainShader.setMat4("view", cam->getInvTransform());
        // mainShader.setMat4("projection", cam->getProjectionMatrix());
        // mainShader.setVec3("ambientLightColor", scene.ambientLight.color);
        // mainShader.setVec3("sunLightColor", scene.sunLight.color * scene.sunLight.intensity);
        // mainShader.setVec3("sunLightDir", scene.sunLight.direction);
        //
        // bool seaHasTexture = scene.sea.diffuseTextureID != 0;
        // mainShader.setBool("hasTexture", seaHasTexture);
        // if (seaHasTexture) {
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, scene.sea.diffuseTextureID);
        //     mainShader.setInt("diffuseTexture", 0);
        // }
        //
        // glBindVertexArray(scene.sea.bufferInfo.VAO);
        // glDrawElements(GL_TRIANGLES, scene.sea.getIndices().size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);
        //------------------------------------------------------------------------------------------
        
        glfwSwapBuffers(renderer.getWindowPointer());
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}
