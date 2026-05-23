#include "Scene.h"
#include "Camera.h"
#include "MeshObject.h"
#include "SunLight.h"

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>

#include <glm/ext/vector_float3.hpp>
#include <vector>


#include <cerrno>
#include <cstring>

Scene::Scene() {
    allMeshObject = vector<MeshObject>();
    camera = Camera();
    // Kinda orange, going down
    sunLight = SunLight(
        glm::vec3(-0.1961, -0.7845, -0.5883),
        glm::vec3(1.0),
        1.2
    );
    ambientLight = AmbientLight(glm::vec3(1.0, 1.0, 1.0));
}

Scene::~Scene() {
}

void Scene::processKeyboardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

MeshObject genCube() {
  // For now just cube
  vector<float> vertices = {0.5, 0.5,  0.5,  -0.5, 0.5,  0.5, -0.5, -0.5,
                            0.5, 0.5,  -0.5, 0.5,  0.5,  0.5, -0.5, -0.5,
                            0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5};

  // Index start with 0 here
  vector<unsigned int> indices = {0, 1, 2, 0, 2, 3, 0, 4, 5, 0, 5, 1, 2, 6, 7, 2, 7, 3, 7, 6, 5, 7, 5, 4, 1, 5, 6, 1, 6, 2, 3, 7, 4, 3, 4, 0};

  MeshObject obj = MeshObject();
  obj.setVertices(vertices);
  obj.setIndices(indices);

  return obj;
}

struct SceneObject {
    MeshObject mesh;
    glm::vec3 position;
    glm::vec3 rotation; // degrees
    glm::vec3 scale;
};
void Scene::objectSetup() {
    MeshObject cube = genCube();
    cube.material = Material();
    // cube.material.color = glm::vec3(0.196, 0.5921, 0.6588);
    cube.material.color = glm::vec3(0.09, 0.757, 1);
    cube.setPosition(glm::vec3(0.0, 0.0, -3.0));
    cube.setRotation(glm::vec3(40.0, 20.0, 30.0));
    cube.recalcTransform();

void loadObjToMeshObject(const std::string& path, MeshObject& obj) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << path << "\n";
        return;
    }

    std::string line;
    std::vector<float> rawVertices;
    std::vector<unsigned int> faceIndices;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            rawVertices.push_back(x); rawVertices.push_back(y); rawVertices.push_back(z);
        } 
        else if (prefix == "f") {
            std::string vertexData;
            while (ss >> vertexData) {
                // Obj format is v/vt/vn, we only want the 'v' part (index)
                size_t slashPos = vertexData.find('/');
                int vIdx = std::stoi(vertexData.substr(0, slashPos)) - 1; // -1 because OBJ is 1-indexed
                faceIndices.push_back(vIdx);
            }
        }
    }
    
    // Now you have your vectors ready for glBufferData!
    obj.setVertices(rawVertices);
    obj.setIndices(faceIndices);
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

        //getting the mesh
        loadObjToMeshObject(objPath, sceneObj.mesh);
        sceneObjects.push_back(sceneObj);
        
        std::cout << "Loaded: " << objPath << " at (" << px << ", " << py << ", " << pz << ")\n";
    }

    return sceneObjects;
}

void Scene::objectSetup() {

    std::filesystem::path exePath = std::filesystem::current_path();
    std::cout << "Working directory: " << exePath << "\n";

    std::string layoutPath = (exePath / "3DScene\\LarpCombat\\scene_layout.txt").string();
    std::cout << "Final path: " << layoutPath << "\n";
    
    auto sceneObjects = loadSceneLayout(layoutPath);


    // if (sceneObjects.empty()) {
    //     std::cerr << "No objects loaded from layout. Check the file and format.\n";
    //     return;
    // }
    
    //applying the transform data to all mesh objects
    for (auto& sceneObj : sceneObjects) {
        sceneObj.mesh.setPosition(sceneObj.position);
        sceneObj.mesh.setRotation(sceneObj.rotation);
        sceneObj.mesh.setScale(sceneObj.scale);
        sceneObj.mesh.recalcTransform();
        
        allMeshObject.push_back(sceneObj.mesh);
    }
}
