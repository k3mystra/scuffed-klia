#include "Input.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "Scene.h"
#include "Camera.h"
#include "MeshObject.h"
#include "SunLight.h"
#include "WindowCallbackData.h"
#include "Model.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cerrno>
#include <cstring>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define check_bit(mods,bit) (mods & bit) == bit


Scene::Scene() {
    initialWindowWidth = 640;
    initialWindowHeight = 360;
    targetAspectRatio = 16.0 / 9.0f;

    isCursorLocked = true;
    lastCursorPos = glm::vec2(-1.0, -1.0);
    dragSpeedMultiplier = 0.1;

    allModels = vector<Model>();
    camera = Camera();
    // Kinda orange, going down
    sunLight = SunLight(
        glm::vec3(-0.1961, -0.7845, 0.5883),
        glm::vec3(1.0),
        1.2
    );
    ambientLight = AmbientLight(glm::vec3(1.0, 1.0, 1.0));
    backgroundColor = glm::vec3(0.1);
}

Scene::~Scene() {
}

bool isPressOrRelease(int act) {
    return act == GLFW_PRESS || act == GLFW_REPEAT;
}

// Reminder that this will be called PER KEYPRESS
// Only 1 keypress will be detected at a time
void Scene::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowCallbackData *data = (WindowCallbackData*)glfwGetWindowUserPointer(window);
    // Ctrl + Q
    if (key == GLFW_KEY_Q && check_bit(mods, GLFW_MOD_CONTROL) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (isCursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isCursorLocked = false;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isCursorLocked = true;
        }
    }
}

void Scene::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec2 currentCursorPos = glm::vec2(xpos, ypos);
    if (lastCursorPos == glm::vec2(-1, -1)) {
        lastCursorPos = currentCursorPos;
        return;
    }

    glm::vec3 rot = camera.getRotation();
    glm::vec2 diff = (currentCursorPos - lastCursorPos) * -dragSpeedMultiplier;
    if (isDragging) {
        camera.setRotation(rot + glm::vec3(diff.y, diff.x, 0.0));
    }
    lastCursorPos = currentCursorPos;
}

void Scene::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    isDragging = (button == GLFW_MOUSE_BUTTON_LEFT && isPressOrRelease(action) && isCursorLocked);
}

void Scene::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    WindowCallbackData *data = (WindowCallbackData*)glfwGetWindowUserPointer(window);

    glm::vec3 camPos = camera.getPosition();

    // We want to zoom in/out of our current local Z-axis
    // NOT the global Z axis
    glm::vec3 zoomVector = camera.getTransform() * glm::vec4(0.0, 0.0, -yoffset, 0.0);
    camera.setPosition(camPos + zoomVector);
}

void Scene::process(float deltaTime) {
    const KeyInfo* wKey = Input::getKey(GLFW_KEY_W);
    const KeyInfo* aKey = Input::getKey(GLFW_KEY_A);
    const KeyInfo* sKey = Input::getKey(GLFW_KEY_S);
    const KeyInfo* dKey = Input::getKey(GLFW_KEY_D);

    glm::vec2 moveInput = glm::vec2(0.0);
    if (wKey != nullptr && wKey->isPressed) {
        moveInput.y = 1;
    }
    else if (sKey != nullptr && sKey->isPressed) {
        moveInput.y = -1;
    }

    if (aKey != nullptr && aKey->isPressed) {
        moveInput.x = -1;
    }
    else if (dKey != nullptr && dKey->isPressed) {
        moveInput.x = 1;
    }

    const float cameraMoveSpeed = 10 * deltaTime;
    glm::vec3 camPos = camera.getPosition();
    glm::vec3 moveVector = camera.getTransform() * glm::vec4(moveInput, 0.0, 0.0) * cameraMoveSpeed;
    camera.setPosition(camPos + moveVector);
}

Model loadObjFile(const std::string& path) {
    Model model;

    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = std::filesystem::path(path).parent_path().string();

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(path, config)) {
        std::cerr << "Error loading OBJ: " << reader.Error() << "\n";
        return model; // return empty model
    }

    if (!reader.Warning().empty())
        std::cout << "OBJ Warning: " << reader.Warning() << "\n";

    auto& attrib    = reader.GetAttrib();
    auto& shapes    = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::string folder = std::filesystem::path(path).parent_path().string();

    // One MeshObject per shape
    for (auto& shape : shapes) {
        MeshObject mesh;
        std::vector<float>        rawVertices;
        std::vector<unsigned int> faceIndices;

        for (auto& index : shape.mesh.indices) {
            rawVertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            rawVertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            rawVertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            faceIndices.push_back(faceIndices.size());
        }

        mesh.setVertices(rawVertices);
        mesh.setIndices(faceIndices);

        // Get material for this shape (use first face's material ID)
        if (!shape.mesh.material_ids.empty()) {
            int matID = shape.mesh.material_ids[0];
            if (matID >= 0 && matID < (int)materials.size()) {
                auto& mat = materials[matID];

                mesh.material.color = glm::vec3(
                    mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]
                );

                if (!mat.diffuse_texname.empty()) {
                    mesh.material.diffuseTexturePath = folder + "/" + mat.diffuse_texname;
                    std::cout << "  Shape '" << shape.name << "' texture: " 
                              << mesh.material.diffuseTexturePath << "\n";
                }
            }
        }

        //DEBUG
        std::cout << "Shape '" << shape.name << "' material_ids: ";
        for (int id : shape.mesh.material_ids)
            std::cout << id << " ";
        std::cout << "\n";

        for (int i = 0; i < materials.size(); i++) {
            std::cout << "Material " << i << ": " << materials[i].name 
                      << " diffuse(" << materials[i].diffuse[0] << "," 
                      << materials[i].diffuse[1] << "," 
                      << materials[i].diffuse[2] << ")"
                      << " texture: " << materials[i].diffuse_texname << "\n";
        }

        std::cout << "  Shape '" << shape.name << "': "
                  << rawVertices.size() / 3 << " vertices, "
                  << faceIndices.size() / 3 << " triangles\n";

        model.meshes.push_back(mesh);
    }

    return model;
}



std::vector<SceneObject> loadSceneLayout(const std::string& layoutPath) {
    std::vector<SceneObject> sceneObjects;
    std::ifstream file(layoutPath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open layout file: " << layoutPath << "\n";
        std::cerr << "Reason: " << std::strerror(errno) << "\n";
        return sceneObjects;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string objPath;
        float px, py, pz, rx, ry, rz, sx, sy, sz;

        ss >> objPath >> px >> py >> pz >> rx >> ry >> rz >> sx >> sy >> sz;

        if (ss.fail()) {
            std::cerr << "Warning: Skipping malformed line: " << line << "\n";
            continue;
        }

        //getting the transform data
        SceneObject sceneObj;
        sceneObj.position = glm::vec3(px, py, pz);
        sceneObj.rotation = glm::vec3(rx, ry, rz);
        sceneObj.scale    = glm::vec3(sx, sy, sz);

        //getting the model
        sceneObj.model = loadObjFile(objPath);
        sceneObjects.push_back(sceneObj);
        
        std::cout << "Loaded: " << objPath << " at (" << px << ", " << py << ", " << pz << ")\n";
    }

    return sceneObjects;
}

void Scene::objectSetup() {
    std::filesystem::path exePath = std::filesystem::current_path();
    std::cout << "Working directory: " << exePath << "\n";

    std::string layoutPath = (exePath / "3DScene/LarpCombat/scene_layout.txt").string();
    std::cout << "Final path: " << layoutPath << "\n";
    
    auto sceneObjects = loadSceneLayout(layoutPath);

    //applying the transform data to all model objects
    for (auto& sceneObj : sceneObjects) {
        sceneObj.model.setPosition(sceneObj.position);
        sceneObj.model.setRotation(sceneObj.rotation);
        sceneObj.model.setScale(sceneObj.scale);
        sceneObj.model.recalcTransform();

        sceneObj.model.meshes[0].material = Material { .color = glm::vec3(0.09, 0.757, 1)};
        allModels.push_back(sceneObj.model);
    }
}
