#include "LoadOBJ.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tiny_obj_loader.h"

#include <iostream>
#include <filesystem>
#include <unordered_map>

static void insertVertexDataIntoMesh(std::vector<float>& vertices, const tinyobj::index_t& idx, const tinyobj::attrib_t& attrib) {
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

static Mesh parseMesh(const tinyobj::mesh_t& mesh_t, const tinyobj::attrib_t& attrib) {
    Mesh mesh;

    // Apparently this is a common trick when you need to combine multiple hashes
    // The magic numbers just have to be large prime numbers
    struct VertexDataHash {
        size_t operator()(const tinyobj::index_t& k) const {
            return (std::hash<int>()(k.vertex_index) * 73856093) ^
                (std::hash<int>()(k.normal_index) * 19349663) ^
                (std::hash<int>()(k.texcoord_index) * 83492791);
        }
    };

    // Mappings from file's indices to new indices
    // Important to use new indices as Mesh::faceIndices will be referring to Mesh::vertices only, not the whole file
    std::unordered_map<tinyobj::index_t, size_t, VertexDataHash> idxMapping = {};

    for (int i = 0; i < mesh_t.indices.size(); i++) {
        tinyobj::index_t idx = mesh_t.indices[i];

        i++;

        // check if already in mapping
        if (auto searchResult = idxMapping.find(idx); searchResult != idxMapping.end())
            // if yes, add the new index into mesh.faceIndices
            mesh.faceIndices.push_back(searchResult->second);
        else {
            // if no, add the VertexData into the mapping, and set new index as (idxMapping.size() - 1), and put into list of vertices
            insertVertexDataIntoMesh(mesh.vertices, idx, attrib);
            idxMapping.insert({ idx, idxMapping.size() - 1 });
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
        // IF they are available
        if (shape.mesh.material_ids.empty())
            continue;

        int matID = shape.mesh.material_ids[0];
        if (matID >= 0 && matID < (int)materials.size()) {
            auto& mat = materials[matID];

            mesh.material.color = glm::vec3(
                mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]
            );

            if (!mat.diffuse_texname.empty())
                mesh.material.diffuseTexturePath = parentFolder + "/" + mat.diffuse_texname;
        }

        model.meshes.push_back(mesh);
    }

    return model;
}
