#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "Components.h"

#include "Render.h"
#include "Transform.h"
#include "EntityDataSystem.h"

#include "SeaPlane.h"
#include "TextureLoader.h"
#include "MeshObject.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "Physics.h"
#include "SkyCube.h"

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

// Initialize scene
Scene scene = Scene();

int main (int argc, char *argv[]) {
    // ==== ECS Migration ====
    // Data init
    // Load all the data needed
    // The systems further processes them
    WorldState worldState = WorldState();
    worldState.sceneConfig.open("scene.txt");

    // New scene file format:
    // # Entity Name
    // The rest of the info
    // <space>
    // ...repeat

    // System inits
    entityDataSytemInit(worldState);
    transformSystemInit(worldState);
    renderSystemInit(worldState, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    // Actual game loop
    // ==== ECS Migration ====

    Shader mainShader = Shader("vertex_shader.glsl", "geometry_shader.glsl", "fragment_shader.glsl");

    // ---- Subject to Change ----
    scene.objectSetup();
    std::vector<Model> allModels = scene.allModels;

    // Put all objects into GPU vertex buffer
    for (Model &model : allModels){
        for (MeshObject &obj : model.meshes) {
            // Vertex Array Object (VAO) to store vertex attributes layout
            // for all VBO
            glGenVertexArrays(1, &obj.bufferInfo.VAO);
            glBindVertexArray(obj.bufferInfo.VAO);

            glGenBuffers(1, &obj.bufferInfo.VBO);
            glBindBuffer(GL_ARRAY_BUFFER, obj.bufferInfo.VBO);
            std::vector vertices = obj.getVertices();
            // Dynamic draw so that we can change em fast later
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

            // Indices shit
            glGenBuffers(1, &obj.bufferInfo.EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.bufferInfo.EBO);
            std::vector indices = obj.getIndices();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

            // Setup vertex attributes
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            
            // TexCoord: location 1, 2 floats
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
    }

        // Sea plane setup
    glGenVertexArrays(1, &scene.sea.bufferInfo.VAO);
    glBindVertexArray(scene.sea.bufferInfo.VAO);

    glGenBuffers(1, &scene.sea.bufferInfo.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, scene.sea.bufferInfo.VBO);
    auto seaVerts = scene.sea.getVertices();
    glBufferData(GL_ARRAY_BUFFER, seaVerts.size() * sizeof(float), seaVerts.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &scene.sea.bufferInfo.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene.sea.bufferInfo.EBO);
    auto seaIdx = scene.sea.getIndices();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, seaIdx.size() * sizeof(unsigned int), seaIdx.data(), GL_STATIC_DRAW);

    // Same layout as models: pos(3) + uv(2) + normal(3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load textures
    scene.sea.diffuseTextureID = loadTexture("3DScene/LarpCombat/water.jpg");
    scene.sea.normalTextureID  = loadTexture("3DScene/LarpCombat/water_normal.jpg");

    for (Model& model : allModels) {
        for (MeshObject& obj : model.meshes) {
            if (!obj.material.diffuseTexturePath.empty()) {
                std::cout << "Loading texture: " << obj.material.diffuseTexturePath << "\n";
                obj.material.textureID = loadTexture(obj.material.diffuseTexturePath);
                std::cout << "  textureID: " << obj.material.textureID << "\n";
            } else {
                std::cout << "No texture for this mesh, using flat color\n";
            }
        }
    }

    SkyCube skybox;
    std::vector<std::string> skyboxFaces = {
        "3DScene/LarpCombat/skybox_right.png",
        "3DScene/LarpCombat/skybox_left.png",
        "3DScene/LarpCombat/skybox_top.png",
        "3DScene/LarpCombat/skybox_bottom.png",
        "3DScene/LarpCombat/skybox_front.png",
        "3DScene/LarpCombat/skybox_back.png"
    };
    skybox.init(skyboxFaces);


    float deltaTime, aggregateDeltaTime = 0;
    float lastFrameTime = 0, currentFrameTime = 0;
    float phyTimeAccumulator = 0.0;
    int frameCount = 0;
    // Main render loop
    while(!glfwWindowShouldClose(worldState.window))
    {
        // Delta time calculations
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // frameCount++;
        // aggregateDeltaTime += deltaTime;

        // if (frameCount == 50) {
        //     cout << "FPS: " << 1.0 / (aggregateDeltaTime / 50.0) << endl;
        //     cout << "Delta time: " << aggregateDeltaTime / 50.0 << endl;
        //     frameCount = 0;
        //     aggregateDeltaTime = 0.0;
        // }

        phyTimeAccumulator += deltaTime;

        // Clear the buffer before next render
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Clear viewport with different color
        glEnable(GL_SCISSOR_TEST);
        glScissor(data.viewportX, data.viewportY, data.viewportWidth, data.viewportHeight);
        glClearColor(
            scene.backgroundColor.r,
            scene.backgroundColor.g,
            scene.backgroundColor.b,
            1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        Camera* cam = &scene.camera;
        cam->recalcTransform();
        skybox.render(cam->getProjectionMatrix(), cam->getInvTransform());

        mainShader.use();

        scene.process(deltaTime);


        for (Model &model : allModels){
            for (MeshObject &obj : model.meshes) {
                Camera* cam = &scene.camera;
                // Recalc transform first
                cam->recalcTransform();
                model.recalcTransform();
            }
        }
        // Physics shit
        while (phyTimeAccumulator >= PHYSICS_TIMESTEP) {
            for (Model &model : allModels) {
                glm::vec3 deltaPos = Physics::updateState(&model.physicsState, PHYSICS_TIMESTEP);
                model.setPosition(model.getPosition() + deltaPos);
            }


            phyTimeAccumulator -= PHYSICS_TIMESTEP;
        }

        for (Model &model : allModels){

            Camera* cam = &scene.camera;
            // Recalc transform first
            cam->recalcTransform();
            model.recalcTransform();

            for (MeshObject &obj : model.meshes) {
                // Pass vertex shader transformations
                mainShader.setMat4("model", model.getTransform());
                mainShader.setMat4("view", cam->getInvTransform());
                mainShader.setMat4("projection", cam->getProjectionMatrix());

                // Pass material color
                mainShader.setVec3("matColor", obj.material.color);

                // Pass ambient light color
                mainShader.setVec3("ambientLightColor", scene.ambientLight.color);

                // Pass diffuse light color (sunlight only for now)
                mainShader.setVec3("sunLightColor", scene.sunLight.color * scene.sunLight.intensity);
                mainShader.setVec3("sunLightDir", scene.sunLight.direction);
                
                // Bind texture if available, otherwise use flat color
                bool hasTexture = obj.material.textureID != 0;
                mainShader.setBool("hasTexture", hasTexture);
                if (hasTexture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, obj.material.textureID);
                    mainShader.setInt("diffuseTexture", 0);
                } else {
                    mainShader.setVec3("matColor", obj.material.color);
                }

                glBindVertexArray(obj.bufferInfo.VAO);

                // Render here
                glDrawElements(GL_TRIANGLES, obj.getIndices().size(), GL_UNSIGNED_INT, 0);
                // Unbind VAO just in case
                glBindVertexArray(0);
            }
        }

        // Snap sea to camera X/Z so it appears infinite
        //-----------------------------------------------------------------------------------------
        // glm::vec3 camPos = scene.camera.getPosition();
        // scene.sea.setPosition(glm::vec3(camPos.x, 0.0f, camPos.z));
        // scene.sea.recalcTransform();
        mainShader.setMat4("model", scene.sea.getTransform());
        mainShader.setMat4("view", cam->getInvTransform());
        mainShader.setMat4("projection", cam->getProjectionMatrix());
        mainShader.setVec3("ambientLightColor", scene.ambientLight.color);
        mainShader.setVec3("sunLightColor", scene.sunLight.color * scene.sunLight.intensity);
        mainShader.setVec3("sunLightDir", scene.sunLight.direction);

        bool seaHasTexture = scene.sea.diffuseTextureID != 0;
        mainShader.setBool("hasTexture", seaHasTexture);
        if (seaHasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, scene.sea.diffuseTextureID);
            mainShader.setInt("diffuseTexture", 0);
        }

        glBindVertexArray(scene.sea.bufferInfo.VAO);
        glDrawElements(GL_TRIANGLES, scene.sea.getIndices().size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        //------------------------------------------------------------------------------------------
        
        glfwSwapBuffers(worldState.window);
        glfwPollEvents();    
    }

    glfwTerminate();
    worldState.sceneConfig.close();
    return 0;
}
