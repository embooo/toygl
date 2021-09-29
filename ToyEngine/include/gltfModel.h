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
        int materialID;

        // Needed to access parts of the element/vertex buffers
        uint32_t startIndices;
        int indicesType;
        uint32_t startVertices;

        uint32_t numIndices;
        uint32_t numVertices;

        Material* material;
    };

    struct Material
    {
        std::string name;
        int id;

        // Metallic-Roughness-Model (default model)
        Texture* baseColorTexture;
        glm::vec4 baseColorFactor;
        bool hasBaseColorTexture = false;

        // Metallic
        Texture* metallicRoughnessTexture;
        float metallicFactor  = 1.0;

        float roughnessFactor = 1.0;
        // Normal
        Texture* normalTexture;
        bool hasNormalTexture = false;
        
        // Ambiant occlusion
        Texture* occlusionTexture;

        // Emissive 
        Texture* emissiveTexture;
        glm::vec3 emissiveFactor;

    };

    struct Image
    {
        Image(const std::string& name, int width, int height, int bits, int channels, int pixelType, const std::vector<unsigned char>& data)
            : width(width), height(height), bitDepth(bits), channels(channels), pixel_type(pixelType), imData(data)
        {
        }

        std::string name;
        int width;
        int height;
        int bitDepth; // bit depth per component
        int channels; // number of channels per pixel
        int pixel_type; // usually UBYTE(bitDepth = 8) or USHORT(bitDepth = 16)

        std::vector<unsigned char> imData;
    };

    struct Sampler
    {
        int id;
        int minFilter;
        int magFilter;
        int wrapS;
        int wrapT;

        Sampler(int minFilter, int magFilter, int wrapS, int wrapT)
            : minFilter(minFilter), magFilter(magFilter), wrapS(wrapS), wrapT(wrapT)
        {

        }
    };

    struct Texture
    {
        GLuint glBufferId;
        std::string name;
        int index;
        Image*   image;
        Sampler* sampler;
        GLuint texCoordSet;
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
        void loadMaterials(tinygltf::Material& material, int materialId);
        void loadTextures(tinygltf::Model& model);
        void loadImages(tinygltf::Model& model);
        void loadSamplers(tinygltf::Model& model);
        void drawRecursively(Node* node, Shader& shader);
        void draw(Shader& shader);
        void drawMeshPrimitives(Node* node, Shader& shader);

        tinygltf::Model tinyglTFModel;

        std::vector<Texture> textures;
        std::vector<GLuint>  textureBufferIds;

        std::unordered_map<int, Material> materials;
        std::vector<Image> images;
        std::vector<Sampler> samplers;

        void loadFromFile(const std::string& filename);
        void createTextureBuffers();
        void traverseNode(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int parentIndex, glTFImporter::Node* parent, const tinygltf::Node& glTFDataNode);
        void processElements(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glTFImporter::Node* parent, const tinygltf::Node& node, glTFImporter::Node* internalNode);


        ~Model();
    };
}