#include "Scene.h"
#include "Camera.h"
#include "MeshObject.h"

#include <vector>


Scene::Scene() {
    allMeshObject = vector<MeshObject>();
    camera = Camera();
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

void Scene::objectSetup() {
    MeshObject cube = genCube();
    cube.setPosition(glm::vec3(0.0, 0.0, -3.0));
    cube.setRotation(glm::vec3(0.0, 20.0, 10.0));
    cube.recalcTransform();

    allMeshObject.push_back(cube);
}
