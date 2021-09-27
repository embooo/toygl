#pragma once

#include <iostream>

#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>

#include "Mesh.h"
#include "OpenGLBuffers.h"
#include "Shader.h"
#include "tiny_gltf.h"

namespace glTFImporter
{

    // Forward declarations
    struct Primitive;
    struct Mesh;
    struct Material;
    struct Texture;
    struct Camera;

    struct Node
    {
        // Each node may refer to a mesh or a camera

        // Local transform with a column-major matrix
        glm::mat4 matrix = glm::identity<glm::mat4>();

        // Local transform with separate matrices for T, R, S
        glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
        glm::quat rotation    = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::vec3 scale       = glm::vec3(1.0, 1.0, 1.0);

        glm::mat4 getLocalTransform();
        glm::mat4 getGlobalTransform();

        std::string name;
        int index;

        Node* parent;
        std::vector<Node*> children;

        Mesh* mesh;
        Camera* camera;

        ~Node() {};
    };

    struct Mesh
    {
        std::string name;
        std::vector<Primitive*> primitives;
    };

    struct Primitive
    {
        // Rendering mode
        // Whether it should rendered as POINTS, LINES or TRIANGLES
        uint32_t mode;

        // Needed to access parts of the element/vertex buffers
        uint32_t startIndices;
        int indicesType;
        uint32_t startVertices;

        uint32_t numIndices;
        uint32_t numVertices;

        // Material that should be used 
        uint32_t materialId;
    };

    struct Material
    {
        std::string name;
        uint32_t index;
        // Metallic-Roughness-Model (default model)
        std::unique_ptr<Texture> baseColorTexture;
        glm::vec4 baseColorFactor;

        std::unique_ptr<Texture> metallicRoughnessTexture;
        float metallicFactor{ 1.0 };
        float roughnessFactor{ 1.0 };

        Texture* normalTexture;
        Texture* occlusionTexture;
        Texture* emissiveTexture;
        glm::vec3 emissiveFactor;
    };


    struct Texture
    {
        double scale;
        uint32_t texcoord;

        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };

    struct Scene
    {
        std::vector<std::unique_ptr<Node>> nodes;
    };


    struct Camera
    {
        Camera() {}
        Camera(double ar, double yfov, double zfar, double znear)
            : aspectRatio(ar), yFov(yfov), zFar(zfar), zNear(znear)
        {}

        std::string name;
        double aspectRatio;
        double yFov;
        double zFar;
        double zNear;
    };





    template<typename T>
    struct Buffer
    {
        Buffer(const T& data) 
            : data(data), stride(0) {}

        T data;
        int stride;
    };



    struct Model
    {
        Model() {};
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texcoord_0;
            glm::vec2 texcoord_1;
        };

        GLenum indicesType;
        //size_t numIndices;
        size_t totalNumVertices = 0;

        VertexArray m_VAO;
        ElementBuffer m_EBO;
        VertexBuffer m_VBO;

        std::vector<glTFImporter::Node*> nodes;

        void drawRecursively(Node* node, Shader& shader);
        void draw(Shader& shader);
        void drawMeshPrimitives(Node* node, Shader& shader);

        tinygltf::Model tinyglTFModel;

        void loadFromFile(const std::string& filename);
        void traverseNode(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int parentIndex, glTFImporter::Node* parent, const tinygltf::Node& glTFDataNode);
        void processElements(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glTFImporter::Node* parent, const tinygltf::Node& node, glTFImporter::Node* internalNode);
    };
}