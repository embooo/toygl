#pragma once

#include <iostream>

#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>

#include "Mesh.h"
#include "OpenGLBuffers.h"
#include "Shader.h"
#include "tiny_gltf.h"

namespace gltf
{
    // Forward declarations
    struct Primitive;
    struct Mesh;
    struct Material;
    struct Texture;
    struct Camera;

    struct BoundingBox
    {
        BoundingBox() :
            min(glm::vec3(std::numeric_limits<float>::max())), max(glm::vec3(std::numeric_limits<float>::min())) {}

        glm::vec3 min;
        glm::vec3 max;

        friend std::ostream& operator<<(std::ostream& os, const BoundingBox& bbox) 
        {  
            os << "min : (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ") max : (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")";

            return os;
        }

        void update(const glm::vec3& p) 
        { 
            min = glm::min(min, p); 
            max = glm::max(max, p); 
        }
    };

    struct Node
    {
        // Each node may refer to a mesh or a camera

        // Local transform with a column-major matrix
        glm::mat4 matrix = glm::identity<glm::mat4>();

        // Local transform with separate matrices for T, R, S
        glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
        glm::quat rotation    = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::vec3 scale       = glm::vec3(1.0, 1.0, 1.0);

        glm::mat4 getLocalTransform() const;
        glm::mat4 getGlobalTransform() ;

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
        uint32_t mode;  // GL_TRIANGLES ...
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

        // Metallic-Roughness workflow : Base color + Roughness + Metallic
        Texture* baseColorTexture;
        glm::vec4 baseColorFactor;
        bool hasBaseColorTexture = false;

        // Metallic-ness  is store in the first component (R)
        // Roughness is store in the second component (G)
        // https://github.com/sbtron/glTF/blob/30de0b365d1566b1bbd8b9c140f9e995d3203226/specification/2.0/README.md#pbrmetallicroughnessmetallicroughnesstexture
        Texture* metallicRoughnessTexture;
        bool hasMetallicRoughnessTexture = false;
        float metallicFactor  = 1.0;
        float roughnessFactor = 1.0;

        // Normal
        Texture* normalTexture;
        bool hasNormalTexture = false;
        
        // Ambiant occlusion
        Texture* occlusionTexture;

        // Emissive 
        Texture* emissiveTexture;
        bool hasEmissiveTexture = false;
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
            glm::vec3 tangeant;
            glm::vec3 bitangeant;
        };

        GLenum indicesType;
        //size_t numIndices;
        size_t totalNumVertices = 0;

        VertexArray   m_VAO;
        ElementBuffer m_EBO;
        VertexBuffer  m_VBO;

        std::vector<gltf::Node*> nodes;
        void loadMaterials(tinygltf::Material& material, int materialId);
        void loadTextures(tinygltf::Model& model);
        void loadImages(tinygltf::Model& model);
        void loadSamplers(tinygltf::Model& model);
        void generateTangeantsBitangeants();

        tinygltf::Model tinyglTFModel;

        std::vector<Texture> textures;
        std::vector<GLuint>  textureBufferIds;

        std::unordered_map<int, Material> materials;
        std::vector<Image> images;
        std::vector<Sampler> samplers;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        BoundingBox bbox;

        void loadFromFile(const std::string& filename);
        void createTextureBuffers();
        void traverseNode(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int parentIndex, gltf::Node* parent, const tinygltf::Node& glTFDataNode);
        void processElements(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, gltf::Node* parent, const tinygltf::Node& node, gltf::Node* internalNode);

        ~Model();
    };
}

