#include <cstddef>
#include <fstream>
#include <filesystem>
#include <functional>
#include <iostream>
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tiny_obj_loader.h"

#include "Render.h"
#include "World.h"
#include "Components.h"


const int VERTICES_PER_TRIANGLE = 3;

void insertVertexDataIntoMesh(std::vector<float>& vertices, const tinyobj::index_t& idx, const tinyobj::attrib_t& attrib) {
    // position of vertices
    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);

    // Normal vector on each vertex
    if (idx.normal_index >= 0) {
        vertices.push_back(attrib.normals[3 * idx.normal_index + 0]);
        vertices.push_back(attrib.normals[3 * idx.normal_index + 1]);
        vertices.push_back(attrib.normals[3 * idx.normal_index + 2]);
    }
    else {
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);
    }

    // texcoords for each vertex
    if (idx.texcoord_index >= 0) {
        vertices.push_back(attrib.texcoords[3 * idx.normal_index + 0]);
        vertices.push_back(attrib.texcoords[3 * idx.normal_index + 1]);
    }
    else {
        vertices.push_back(0);
        vertices.push_back(1);
    }
}

Mesh parseMesh(const tinyobj::mesh_t& mesh_t, const tinyobj::attrib_t& attrib) {
    Mesh mesh;

    struct VertexDataHash {
        size_t operator()(const tinyobj::index_t& k) const {
            return (std::hash<int>()(k.vertex_index) * 73856093) ^
                (std::hash<int>()(k.normal_index) * 19349663) ^
                (std::hash<int>()(k.texcoord_index) * 83492791);
        }
    };

    size_t indexOffset = 0;

    // Mappings from file's indices to new indices
    // Important to use new indices as Mesh::faceIndices will be referring to Mesh::vertices only, not the whole file
    std::unordered_map<tinyobj::index_t, size_t, VertexDataHash> idxMapping = {};

    // Loop over faces
    // A bit of hack; num_face_vertices is array containing number of vertices for each face
    // So the size of that array is equal to no. of faces
    for (int f = 0; f < mesh_t.num_face_vertices.size(); f++) {

        // Loop over vertices per face
        // Possible to use constant here since we force triangulate
        for (int vf = 0; vf < VERTICES_PER_TRIANGLE; vf++) {
            tinyobj::index_t idx = mesh_t.indices[indexOffset];

            indexOffset++;

            // check if already in mapping
            if (auto searchResult = idxMapping.find(idx); searchResult != idxMapping.end()) {
                // if yes, add the new index into mesh.faceIndices
                mesh.faceIndices.push_back(searchResult->second);
            }
            else {
                // if no, add the VertexData into the mapping, and set new index as (idxMapping.size() - 1), and put into list of vertices
                insertVertexDataIntoMesh(mesh.vertices, idx, attrib);
                idxMapping.insert({ idx, idxMapping.size() - 1 });
            }
        }
    }

    return mesh;
}

Model loadObjFile(const std::string& path) {
    Model model;

    std::string parentFolder = std::filesystem::path(path).parent_path().string();

    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = parentFolder;
    // Force all faces to be triangle (cuz we ain't handling anything other than that)
    config.triangulate = true;

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(path, config)) {
        std::cerr << "Error loading OBJ: " << reader.Error() << "\n";
        return model; // return empty model
    }

    if (!reader.Warning().empty())
        std::cout << "OBJ Warning: " << reader.Warning() << "\n";

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // One Mesh per shape
    for (auto& shape : shapes) {
        Mesh mesh = parseMesh(shape.mesh, attrib);

        // Get material for this shape (use first face's material ID)
        if (!shape.mesh.material_ids.empty()) {
            int matID = shape.mesh.material_ids[0];
            if (matID >= 0 && matID < (int)materials.size()) {
                auto& mat = materials[matID];

                mesh.material.color = glm::vec3(
                    mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]
                );

                if (!mat.diffuse_texname.empty()) {
                    mesh.material.diffuseTexturePath = parentFolder + "/" + mat.diffuse_texname;
                    std::cout << "  Shape '" << shape.name << "' texture: " 
                              << mesh.material.diffuseTexturePath << "\n";
                }
            }
        }

        //DEBUG
        // std::cout << "Shape '" << shape.name << "' material_ids: ";
        // for (int id : shape.mesh.material_ids)
        //     std::cout << id << " ";
        // std::cout << "\n";

        // for (int i = 0; i < materials.size(); i++) {
        //     std::cout << "Material " << i << ": " << materials[i].name 
        //               << " diffuse(" << materials[i].diffuse[0] << "," 
        //               << materials[i].diffuse[1] << "," 
        //               << materials[i].diffuse[2] << ")"
        //               << " texture: " << materials[i].diffuse_texname << "\n";
        // }

        // std::cout << "  Shape '" << shape.name << "': "
        //           << rawVertices.size() / 3 << " vertices, "
        //           << faceIndices.size() / 3 << " triangles\n";

        std::cout << "Shape '" << shape.name << "': "
          << "has UVs: " << !attrib.texcoords.empty()
          << " has normals: " << !attrib.normals.empty() << "\n";

        model.meshes.push_back(mesh);
    }

    return model;
}



static void parseSceneFile(WorldState& worldState) {
    std::ifstream& sceneFile = worldState.sceneConfig;
    sceneFile.seekg(0);

    std::string line;
    while (std::getline(sceneFile, line)) {
        if (line[0] != '#')
            continue;

        // TODO: get the offset

        // Get model
        // Line format: path to OBJ file (enclosed in quotes)
        Model model = Model();
    }
}



static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowCallbackData *data = (WindowCallbackData*)glfwGetWindowUserPointer(window);
    float targetAspectRatio = data->targetAspectRatio;

    float windowAspect = (float)width / (float)height;
    
    int viewportWidth, viewportHeight;
    int viewportX = 0, viewportY = 0;
    
    if (windowAspect > targetAspectRatio) {
        // Window is wider than target - add bars on sides
        viewportHeight = height;
        viewportWidth = (int)(height * targetAspectRatio);
        viewportX = (width - viewportWidth) / 2;
    } else {
        // Window is taller than target - add bars on top/bottom
        viewportWidth = width;
        viewportHeight = (int)(width / targetAspectRatio);
        viewportY = (height - viewportHeight) / 2;
    }
    
    data->viewportX = viewportX;
    data->viewportY = viewportY;
    data->viewportWidth = viewportWidth;
    data->viewportHeight = viewportHeight;

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

GLFWwindow* setupGlfwWindow(WindowCallbackData* data, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(
            initialWindowWidth, initialWindowHeight, "Larp Combat", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    
    glfwSetWindowUserPointer(window, data);
    // Resize viewport on windows resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    return window;
}

void renderSystemInit(WorldState& worldState, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
     // By default already set to screen size, but useful if we resize the windows later
    glViewport(0, 0, initialWindowWidth, initialWindowHeight);
    // Pass WindowCallbackData for use by any callbacks
    WindowCallbackData data {
        .targetAspectRatio = 16.0 / 9.0,
        .viewportX = 0,
        .viewportY = 0,
        .viewportWidth = initialWindowWidth,
        .viewportHeight = initialWindowHeight,
        .deltaTime = 0
    };

    worldState.window = setupGlfwWindow(&data, initialWindowWidth, initialWindowHeight);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        exit(1);
    }

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);

    parseSceneFile(worldState);
}
