#pragma once
#include "PCH.h"

#include "Mesh.h"
#include "OpenGLBuffers.h"
#include "Shader.h"
#include "tiny_gltf.h"

namespace opengltf
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
        uint32_t firstIndex;
        int indicesType;
        uint32_t startVertices;

        uint32_t indexCount;
        uint32_t numVertices;

        Material* material;
    };

    struct Material
    {
        const char *name;
        unsigned int id;
        // Metallic-Roughness workflow : Base color + Roughness + Metallic

        // Metallic-ness  is store in the first component (R)
        // Roughness is store in the second component (G)
        // https://github.com/sbtron/glTF/blob/30de0b365d1566b1bbd8b9c140f9e995d3203226/specification/2.0/README.md#pbrmetallicroughnessmetallicroughnesstexture

        Texture* base_color_tex     = nullptr;
        Texture* metallic_rough_tex = nullptr;
        Texture* normal_tex         = nullptr;
        Texture* occlusion_tex      = nullptr;
        Texture* emissive_tex       = nullptr;

        glm::vec3 emissive_factor;
        glm::vec4 base_color_factor;

        float metallic_factor  = 0.0;
        float roughness_factor = 0.0;
    };

    struct ShaderMaterial
    {
        int32_t albedo     = 0;
        int32_t normal     = 0;
        int32_t specular   = 0;
        int32_t emissive   = 0;
        int32_t occlusion  = 0;
        int32_t metalRough = 0;

        //glm::vec3 emissiveFactor;
        //glm::vec4 baseColorFactor;

        //float metallicFactor;
        //float roughnessFactor;
    };

    struct Image
    {
        int width;
        int height;
        int bitDepth;   // bit depth per component
        int nbChannels; // number of channels per pixel
        int pixelType;  // type of each component usually UBYTE(bitDepth = 8) or USHORT(bitDepth = 16)
        const char *name;
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
        const char* name;
        unsigned int id;
        unsigned int glBufferId;
        unsigned int texcoordSet;
        Image*   image;
        Sampler* sampler;
    };



    struct Scene
    {
        std::vector<std::unique_ptr<Node>> nodes;
    };


    struct Camera
    {
        const char* name;
        double aspectRatio;
        double yFov;
        double zFar;
        double zNear;
        glm::vec3 translation;
    };


    template<typename T>
    struct Buffer
    {
        Buffer(const T& data) 
            : data(data), stride(0) {}

        T data;
        int stride;
    };

    struct Vertex
    {
        glm::vec3 position;
        float pad0;
        glm::vec3 normal;
        float pad1;
        glm::vec4 tangeant;
        glm::vec2 texcoord_0;
        glm::vec2 pad3;
        glm::vec2 texcoord_1;
        glm::vec2 pad4;

        int32_t materialID;
    };

    struct Model
    {
        Model() {};

        struct Vertices
        {
            std::vector<Vertex> data;
            
            size_t size() { return data.size(); }
            void   add(Vertex& v) { return data.push_back(v); }
            auto   getData() { return data.data(); }
        };

        GLenum indicesType;
        //size_t numIndices;
        size_t totalNumVertices = 0;

        VertexArray   m_VAO;
        ElementBuffer m_EBO;
        VertexBuffer  m_VBO;

        std::vector<opengltf::Node*> nodes;
        void loadMaterials(const tinygltf::Model& model);
        void loadTextures(tinygltf::Model& model);
        void loadImages(tinygltf::Model& model);
        void loadSamplers(tinygltf::Model& model);

        tinygltf::Model tinyglTFModel;

        std::vector<Texture> textures;
        std::vector<GLuint>  textureBufferIds;

        std::vector<Material> materials;
        std::vector<Image> images;
        std::vector<Sampler> samplers;

        Vertices vertices;
        std::vector<unsigned int> indices;

        BoundingBox bbox;

        glm::mat4 matModel;

        glm::mat4& modelMat();

        std::vector<Camera> m_Cameras;

        unsigned int textureArray;
        GLuint createTextureArray(unsigned int textureUnit);

        SSBO ssbo_materials;
        SSBO buildMaterialSSBO(unsigned int bindingPoint);

        void loadFromFile(const std::string& filename);
        void traverseNode(tinygltf::Model& model,    Vertices& vertices, std::vector<unsigned int>& indices, int parentIndex, opengltf::Node* parent, const tinygltf::Node& glTFDataNode);
        void processElements(tinygltf::Model& model, Vertices& vertices, std::vector<unsigned int>& indices, opengltf::Node* parent, const tinygltf::Node& node, opengltf::Node* internalNode);

        ~Model();
    };
}

