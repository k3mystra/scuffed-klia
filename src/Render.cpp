#include <cstddef>
#include <glm/fwd.hpp>
#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <utility>

#include "Render.h"
#include "Components.h"
#include "World.h"
#include "Shader.h"
#include "TextureLoader.h"


const std::string FRAGMENT_SHADER_SRC_PATH = "default_shaders/fragment_shader.glsl";
const std::string GEOMETRY_SHADER_SRC_PATH = "default_shaders/geometry_shader.glsl";
const std::string VERTEX_SHADER_SRC_PATH = "default_shaders/vertex_shader.glsl";

const std::string SKYBOX_FRAGMENT_SHADER_SRC_PATH = "default_shaders/skybox_fragment.glsl";
const std::string SKYBOX_VERTEX_SHADER_SRC_PATH = "default_shaders/skybox_vertex.glsl";

const glm::vec3 DEFAULT_BG = glm::vec3(0.2, 0.2, 0.2);

typedef std::pair<Camera, Transform> CameraTransformData;
typedef std::pair<Model, Transform> ModelTransformData;


void APIENTRY openGLDebugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

        case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

        default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

        case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

        case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

        default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

        case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

        default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n",
            id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
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

static GLFWwindow* setupGlfwWindow(WindowCallbackData* data, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
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

static void initializeMesh(Mesh& mesh) {
    // === Load vertex & face data

    // Vertex Array Object (VAO) to store vertex attributes layout
    // for all VBO
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    // Dynamic draw so that we can change em fast later
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_DYNAMIC_DRAW);

    // Indices shit
    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faceIndices.size() * sizeof(unsigned int), mesh.faceIndices.data(), GL_DYNAMIC_DRAW);

    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // TexCoord: location 1, 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // === Load Textures ===
    if (!mesh.material.diffuseTexturePath.empty()) {
        std::cout << "Loading texture: " << mesh.material.diffuseTexturePath << "\n";
        mesh.material.textureID = loadTexture(mesh.material.diffuseTexturePath);
        std::cout << "  textureID: " << mesh.material.textureID << "\n";
    } else {
        std::cout << "No texture for this mesh, using flat color\n";
    }
}


void RenderSystem::initializeComponents(World& world) {
    for (Model& model : world.modelList) {
        model.shader = getDefaultShader();

        for (Mesh& mesh : model.meshes) {
            initializeMesh(mesh);
        }
    }

    // Assume only 1 exists
    Camera* cam = &world.cameraList[0];
    cam->projectionMatrix = glm::perspective(glm::radians(cam->fov), cam->aspectRatio, cam->nearPlane, cam->farPlane);
}

RenderSystem::RenderSystem(World& world, unsigned int initialWindowWidth, unsigned int initialWindowHeight) {
     // By default already set to screen size, but useful if we resize the windows later
    glViewport(0, 0, initialWindowWidth, initialWindowHeight);
    // Pass WindowCallbackData for use by any callbacks
    windowCallbackData = {
        .targetAspectRatio = 16.0 / 9.0,
        .viewportX = 0,
        .viewportY = 0,
        .viewportWidth = initialWindowWidth,
        .viewportHeight = initialWindowHeight,
        .deltaTime = 0
    };

    world.window = setupGlfwWindow(&windowCallbackData, initialWindowWidth, initialWindowHeight);

    // Init. GLEW to query the driver and actually load OpenGL library
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        exit(1);
    }

    // OpenGL Functions to enable
    glEnable(GL_DEPTH_TEST);

    #ifdef ENABLE_OPENGL_DEBUG
    // Debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openGLDebugCallback, 0);
    #endif

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initializeComponents(world);
}

Shader RenderSystem::getDefaultShader() {
    if (_defaultShader.programID != 0)
        return _defaultShader;

    _defaultShader.vertexShaderSrcPath = VERTEX_SHADER_SRC_PATH;
    _defaultShader.geometryShaderSrcPath = GEOMETRY_SHADER_SRC_PATH;
    _defaultShader.fragmentShaderSrcPath = FRAGMENT_SHADER_SRC_PATH;

    shader_utils::initializeShader(_defaultShader);
    return _defaultShader;
}

void RenderSystem::resetBuffer() {
    // Clear the buffer before next render
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Clear viewport with different color
    glEnable(GL_SCISSOR_TEST);
    glScissor(
        windowCallbackData.viewportX,
        windowCallbackData.viewportY,
        windowCallbackData.viewportWidth,
        windowCallbackData.viewportHeight
    );
    glClearColor(
        DEFAULT_BG.r,
        DEFAULT_BG.g,
        DEFAULT_BG.b,
        1.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

static void renderObj(
    const ModelTransformData& modelData,
    const CameraTransformData& camData,
    const World& world
) {

    glUseProgram(modelData.first.shader.programID);

    for (const Mesh& mesh : modelData.first.meshes) {
        shader_utils::setMat4(modelData.first.shader, "model", modelData.second.matrix);
        shader_utils::setMat4(modelData.first.shader, "view", camData.second.invMatrix);
        shader_utils::setMat4(modelData.first.shader, "projection", camData.first.projectionMatrix);

        shader_utils::setVec3(modelData.first.shader, "matColor", mesh.material.color);

        shader_utils::setVec3(modelData.first.shader, "ambientLightColor", world.ambientLight.color);

        shader_utils::setVec3(modelData.first.shader, "sunLightColor", world.sunlight.color);
        shader_utils::setVec3(modelData.first.shader, "sunLightDir", world.sunlight.direction);

        bool hasTexture = mesh.material.textureID != 0;
        shader_utils::setBool(modelData.first.shader, "hasTexture", hasTexture);
        if (hasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.material.textureID);
            shader_utils::setInt(modelData.first.shader, "diffuseTexture", 0);
        }

        glBindVertexArray(mesh.VAO);

        // Actually rendering
        // crazy
        glDrawElements(GL_TRIANGLES, mesh.faceIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void RenderSystem::render(World& world) {
    resetBuffer();

    // A whole lot of slow code
    // Would be nice to move to Unity DOTS archetype system

    CameraTransformData camObjectData;
    // Find camera data
    for (int e = 0; e < world.totalEntity; e++) {
        auto camSearch = world.cameraIndex.find(e);
        if (camSearch == world.cameraIndex.end())
            continue;

        auto transformSearch = world.transformIndex.find(e);
        if (transformSearch == world.transformIndex.end())
            continue;

        camObjectData.first = world.cameraList[camSearch->second];
        camObjectData.second = world.transformList[transformSearch->second];
        break;
    }

    // Iterate thru all entity, since we need multiple components from each entity
    ModelTransformData modelObjectData;
    for (int e = 0; e < world.totalEntity; e++) {
        auto modelSearch = world.modelIndex.find(e);
        if (modelSearch == world.modelIndex.end())
            continue;

        auto transformSearch = world.transformIndex.find(e);
        if (transformSearch == world.transformIndex.end())
            continue;

        modelObjectData.first = world.modelList[modelSearch->second];
        modelObjectData.second = world.transformList[transformSearch->second];

        renderObj(modelObjectData, camObjectData, world);
    }
}
