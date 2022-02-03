#pragma once
#include "PCH.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "gltfModel.h"
#include <glm/gtc/type_ptr.hpp>

glm::mat4& opengltf::Model::modelMat()
{
    return matModel;
}

void opengltf::Model::loadFromFile(const std::string& filename)
{
    std::string error;
    std::string warning;

    tinygltf::TinyGLTF tinyglTFLoader;
    tinygltf::Model tinyglTFModel;

    bool loaded = false;
    bool binaryType = false;

    std::string extension = filename.substr(filename.find_last_of('.'));

    if(extension == ".glb") binaryType = true;

    loaded = binaryType ? tinyglTFLoader.LoadBinaryFromFile(&tinyglTFModel, &error, &warning, filename) : tinyglTFLoader.LoadASCIIFromFile(&tinyglTFModel, &error, &warning, filename);

    if(loaded)
    {
        std::cout << "Loaded " << filename << " successfully. \n";

        loadTextures (tinyglTFModel);
        loadMaterials(tinyglTFModel);
        
        /////// 1. Extract model data from nodes 
        
        for(const tinygltf::Scene& scene : tinyglTFModel.scenes)
        {
            for(int nodeId : scene.nodes)
            {
               const tinygltf::Node& node = tinyglTFModel.nodes.at(nodeId);

               traverseNode(tinyglTFModel, vertices, indices, nodeId, nullptr, node);
            }
        }
        
        std::cerr << "Bounding box : " << bbox << std::endl;

        // Set the transform of the whole model
        matModel = nodes.front()->getGlobalTransform();

        /////// 2. Setup OpenGL buffers / textures
        ssbo_materials = buildMaterialSSBO(0);
        textureArray = createTextureArray(0);

        // Create Vertex array object
        // Storing layout, format of vertex data and buffer objects
        m_VAO.generate();
        m_VAO.bind();

        // Create Vertex buffer object
        // Stores vertex data
        m_VBO.generate();
        m_VBO.bind();
        m_VBO.initData(sizeof(Vertex) * vertices.size(), vertices.getData(), GL_DYNAMIC_DRAW);

        //// Create Element buffer object
        //// Stores vertex indices
        m_EBO.generate();
        m_EBO.bind();
        m_EBO.initData(indices, GL_UNSIGNED_INT, GL_DYNAMIC_DRAW);

        // Here we specify the layout and format of the vertex data 
        // Stride is the amount of bytes between consecutive vertex attributes of the same kind
        // Vertex attributes are interleaved so we need to specify a stride != 0
        const size_t stride = sizeof(Vertex);

        // 0, 1, 2, 3 are indices for Position, Normal, Texcoord and Color respectively
        m_VAO.setupAttribute(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        m_VAO.setupAttribute(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
        m_VAO.setupAttribute(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, tangeant));
        m_VAO.setupAttribute(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texcoord_0));
        m_VAO.setupAttribute(4, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texcoord_1));
        m_VAO.setupAttribute(5, 1, GL_INT,   GL_FALSE, stride, (void*)offsetof(Vertex, materialID)); // /!\ use glVertexAttribIPointer for integral values

        // Finished modifying our buffers and array
        m_VAO.unbind();
        m_VBO.unbind();
        m_EBO.unbind();
    }
    else 
    {
        std::cerr << "Could not load file : " << error << "\n";
    }
}

SSBO opengltf::Model::buildMaterialSSBO(unsigned int bindingPoint)
{
    std::vector<ShaderMaterial> sm;

    for (Material& mat : materials)
    {
        bool notEmpty = false;

        ShaderMaterial smat;

        if (mat.base_color_tex) 
        { 
            notEmpty = true;
            smat.albedo = mat.base_color_tex->id; 
            //smat.baseColorFactor = mat.baseColorFactor; 
        }
        // --------------------------------------------
        if (mat.emissive_tex)  
        { 
            notEmpty = true;
            smat.emissive = mat.emissive_tex->id; 
            //smat.emissiveFactor = mat.emissiveFactor;
        }
        // --------------------------------------------

        if (mat.normal_tex) 
        { 
            notEmpty = true;
            smat.normal = mat.normal_tex->id; 
        }
        // --------------------------------------------

        if (mat.occlusion_tex) 
        { 
            notEmpty = true;
            smat.occlusion = mat.occlusion_tex->id; 
        }
        // --------------------------------------------

        if (mat.metallic_rough_tex)
        {
            notEmpty = true;
            smat.metalRough = mat.metallic_rough_tex->id;
            //smat.metallicFactor  = mat.metallicFactor;
            //smat.roughnessFactor = mat.roughnessFactor;
        }

        if(notEmpty)
            sm.push_back(smat);
    }

    SSBO materials;
    materials.generate();
    materials.bind();
    materials.bindBase(bindingPoint);
    materials.setData(sm.size() * sizeof(ShaderMaterial), sm.data(), GL_DYNAMIC_DRAW);

    materials.unbind();

    return materials;
}

/// <summary>
/// This function converts a glTF node into an internal representation
/// and sends it to be processed.
/// </summary>
/// <param name="model"> Contains glTF data parsed/extracted by tinygltf </param>
/// <param name="parentIndex"> Index of the parent node in the glTF file </param>
/// <param name="parent"> Internal representation of a glTF node </param>
/// <param name="glTFDataNode"> Tinygltf representation of a glTF node </param>
void opengltf::Model::traverseNode(tinygltf::Model& model, Vertices& vertices, std::vector<unsigned int>& indices, int nodeIndex, opengltf::Node* parent, const tinygltf::Node& glTFDataNode)
{
    opengltf::Node* current(new Node{});
    
    processElements(model, vertices, indices, parent, glTFDataNode, current);

    if (!glTFDataNode.children.empty())
    {
        for (int childId : glTFDataNode.children)
        {
            traverseNode(model, vertices, indices, childId, current, model.nodes.at(childId));
        }
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="model"></param>
/// <param name="parent"></param>
/// <param name="node"></param>


void opengltf::Model::processElements(tinygltf::Model& model, Vertices& vertices, std::vector<unsigned int>& indices, opengltf::Node* parent, const tinygltf::Node& node, opengltf::Node* newNode)  // processNode
{
    newNode->name = node.name;
    // Extract local transform
    if (node.matrix.size() == 16)
    {
        newNode->matrix = glm::make_mat4(node.matrix.data());
    }

    if (node.translation.size() == 3) 
    {
        newNode->translation = glm::make_vec3(node.translation.data());
    }

    if (node.rotation.size() == 4) 
    {
        newNode->rotation = glm::make_quat(node.rotation.data());
    }

    if (node.scale.size() == 3) 
    {
        newNode->scale = glm::make_vec3(node.scale.data());
    }

    // Check if node refers to a mesh or a camera
    if (node.mesh != -1)
    {
        newNode->mesh = new opengltf::Mesh();
        const tinygltf::Mesh& glTFMesh = model.meshes.at(node.mesh);
        newNode->mesh->name = glTFMesh.name;
        
        for(const tinygltf::Primitive& p : glTFMesh.primitives)
        {
            Primitive* primitive = new Primitive{};

            primitive->startVertices = vertices.size(); 
            primitive->firstIndex    = indices.size();
            primitive->materialID    = p.material;

            if (p.mode > -1) primitive->mode = p.mode;

            std::cout << "Primitive mat id : " << primitive->materialID << std::endl;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Extract Vertex Attributes 
            /// 1. Extract the buffers and informations regarding layout

            const float* positionBuffer  = nullptr;

            const float* normalBuffer    = nullptr;

            const float* tangentBuffer = nullptr;

            const float* texcoord0Buffer = nullptr;

            const float* texcoord1Buffer = nullptr;
            
            if(p.attributes.find("POSITION") != p.attributes.end())
            {
                const tinygltf::Accessor& accessor      = model.accessors.at(p.attributes.at("POSITION"));
                const tinygltf::BufferView& bufferView  = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer          = model.buffers.at(bufferView.buffer);

                primitive->numVertices = accessor.count;
                totalNumVertices += accessor.count;
                positionBuffer = reinterpret_cast<const float*>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
            }

            //assert(positionBuffer != nullptr, ("error : model doesn't contain vertex positions."));

            if(p.attributes.find("NORMAL") != p.attributes.end())
            {
                const tinygltf::Accessor& accessor      = model.accessors.at(p.attributes.at("NORMAL"));
                const tinygltf::BufferView& bufferView  = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer          = model.buffers.at(bufferView.buffer);

                normalBuffer     = reinterpret_cast<const float*>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
            }

            if(p.attributes.find("TEXCOORD_0") != p.attributes.end())
            {

                const tinygltf::Accessor& accessor     = model.accessors.at(p.attributes.at("TEXCOORD_0"));
                const tinygltf::BufferView& bufferView = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer         = model.buffers.at(bufferView.buffer);
                texcoord0Buffer = reinterpret_cast<const float*>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
            }

            if(p.attributes.find("TEXCOORD_1") != p.attributes.end())
            {
                const tinygltf::Accessor& accessor     = model.accessors.at(p.attributes.at("TEXCOORD_1"));
                const tinygltf::BufferView& bufferView = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer         = model.buffers.at(bufferView.buffer);

                texcoord1Buffer = reinterpret_cast<const float*>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
            }

            if (p.attributes.find("TANGENT") != p.attributes.end())
            {
                const tinygltf::Accessor& accessor     = model.accessors.at(p.attributes.at("TANGENT"));
                const tinygltf::BufferView& bufferView = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer         = model.buffers.at(bufferView.buffer);

                tangentBuffer = reinterpret_cast<const float*>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
            }

            ////////////////////////////////////////////////////////////////////////////////// Extract Indices
            if(p.indices != -1) 
            {
                const tinygltf::Accessor& accessor     = model.accessors.at(p.indices);
                const tinygltf::BufferView& bufferView = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer         = model.buffers.at(bufferView.buffer);

                const void* indicesBuffer              = &(buffer.data[bufferView.byteOffset + accessor.byteOffset]); 

                primitive->indexCount   = accessor.count;

                switch(accessor.componentType)
                {
                    // 5125 
                    case GL_UNSIGNED_INT:
                    {
                        primitive->indicesType = GL_UNSIGNED_INT;
                        const unsigned int* bufferIndices = static_cast<const unsigned int*>(indicesBuffer);
                        for (size_t i = 0; i < primitive->indexCount; ++i)
                        {
                            indices.push_back(primitive->startVertices + bufferIndices[i]);
                        }
                        break;
                    }

                    // 5123 
                    case GL_UNSIGNED_SHORT:
                    {
                        primitive->indicesType = GL_UNSIGNED_SHORT;
                        const unsigned short* bufferIndices = static_cast<const unsigned short*>(indicesBuffer);
                        for (size_t i = 0; i < primitive->indexCount; ++i)
                        {
                            indices.push_back(primitive->startVertices + bufferIndices[i]);
                        }
                        break;
                    }

                    // 5121 
                    case GL_UNSIGNED_BYTE:
                    {
                        primitive->indicesType = GL_UNSIGNED_BYTE;
                        const unsigned char* bufferIndices = static_cast<const unsigned char*>(indicesBuffer);
                        for (size_t i = 0; i < primitive->indexCount; ++i)
                        {
                            indices.push_back(primitive->startVertices + bufferIndices[i]);
                        }
                        break;
                    }

                    default: std::cerr << "Unknown componentType for indices\n";
                }
            }

            ////////////////////////////////////////////////////////////////////////////////// Extract material ID

            // Update vertex buffer
            for (size_t i = 0; i < primitive->numVertices; ++i)
            {
                Vertex vertex;

                // Set vertex attributes
                assert(positionBuffer);
                vertex.position                         = glm::make_vec3(&positionBuffer [i * 3]);
                if (normalBuffer)     vertex.normal     = glm::make_vec3(&normalBuffer   [i * 3]);
                if (tangentBuffer)    vertex.tangeant   = glm::make_vec4(&tangentBuffer  [i * 4]);
                if (texcoord0Buffer)  vertex.texcoord_0 = glm::make_vec2(&texcoord0Buffer[i * 2]);
                if (texcoord1Buffer)  vertex.texcoord_1 = glm::make_vec2(&texcoord1Buffer[i * 2]);

                vertex.materialID = primitive->materialID;

                bbox.update(vertex.position);

                vertices.add(vertex);
            }

            newNode->mesh->primitives.push_back(primitive);
        }

    }
    else if (node.camera != -1)
    {
        const tinygltf::Camera& camera = model.cameras.at(node.camera);

        newNode->camera = new Camera;
        *newNode->camera = {
            camera.name.c_str(),
            camera.perspective.aspectRatio,
            camera.perspective.yfov,
            camera.perspective.zfar,
            camera.perspective.znear,
        };

        if (node.translation.size() == 3)
        {
            newNode->camera->translation = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
        }

    }

    // Node hierarchy
    if (parent)
    {
        newNode->parent = parent;
        parent->children.push_back(newNode);
    }
    else
    {
        nodes.push_back(newNode);
    }
}

void opengltf::Model::loadMaterials(const tinygltf::Model& model)
{
        
        for (int i = 0; i < model.materials.size(); ++i)
        {
            const tinygltf::Material& material = model.materials[i];
            const tinygltf::PbrMetallicRoughness& pbr = material.pbrMetallicRoughness;

            opengltf::Material mat;
            mat.name = material.name.c_str();
            mat.id   = i;

            printf("Loaded material : %s (%i) \n", mat.name, mat.id);

            // https://substance3d.adobe.com/tutorials/courses/the-pbr-guide-part-2
            // PBR metallic-roughness workflow : Base Color + [Roughness & Metallic]
            // Common to all workflow : Ambient occlusion map, Normal map, Height map, emissive

            // Base color texture
            if (pbr.baseColorTexture.index != -1)
            {
                mat.base_color_tex              = &textures[pbr.baseColorTexture.index];
                mat.base_color_tex->id          = pbr.baseColorTexture.index;
                mat.base_color_factor           = glm::make_vec4(pbr.baseColorFactor.data());
                mat.base_color_tex->texcoordSet = pbr.baseColorTexture.texCoord;
            }

            // Metallic-Roughness texture
            if (pbr.metallicRoughnessTexture.index != -1)
            {
                mat.metallic_rough_tex              = &textures[pbr.metallicRoughnessTexture.index];
                mat.metallic_rough_tex->id          = pbr.metallicRoughnessTexture.index;
                mat.metallic_rough_tex->texcoordSet = pbr.metallicRoughnessTexture.texCoord;
                mat.roughness_factor                = pbr.roughnessFactor;
                mat.metallic_factor                 = pbr.metallicFactor;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////// Common to all workflows 

            // Emissive
            if (material.emissiveTexture.index != -1)
            {
                mat.emissive_tex              = &textures[material.emissiveTexture.index];
                mat.emissive_tex->id          = material.emissiveTexture.index;
                mat.emissive_tex->texcoordSet = material.emissiveTexture.texCoord;
                mat.emissive_factor           = glm::make_vec3(material.emissiveFactor.data());
            }

            // Normal
            if (material.normalTexture.index != -1)
            {
                mat.normal_tex              = &textures[material.normalTexture.index];
                mat.normal_tex->id          = material.normalTexture.index;
                mat.normal_tex->texcoordSet = material.normalTexture.texCoord;
            }

            // Occlusion
            if (material.occlusionTexture.index != -1)
            {
                mat.occlusion_tex              = &textures[material.occlusionTexture.index];
                mat.occlusion_tex->id          = material.occlusionTexture.index;
                mat.occlusion_tex->texcoordSet = material.occlusionTexture.texCoord;
            }

            materials.push_back(mat);
        }

}

GLuint opengltf::Model::createTextureArray(unsigned int unit)
{
    GLuint  texture_array;
    GLsizei base_width    = textures[0].image->width, 
            base_height   = textures[0].image->height;
    GLsizei mipLevelCount = 1;
    GLsizei mipLevel = 0;
    GLsizei texture_count = images.size();

    glGenTextures(1, &texture_array);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);
    
    // sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, /*mipmap*/ mipLevel, GL_RGBA, base_width, base_height, texture_count, /*border*/ 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    for (int i = 0; i < images.size(); i++)
    {
        if (images[i].width == base_width && images[i].height == base_height)
        {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, /* mipmap */ mipLevel,
                /* x offset */ 0, /* y offset */ 0, /* z offset == index */ i,
                base_width, base_height, 1,
                images[i].nbChannels, images[i].pixelType, images[i].imData.data());
        }

    }

    printf("Created texture array: %dx%dx%d (%dMb)\n", base_width, base_height, texture_count, 4 * base_width * base_height * texture_count / 1024 / 1024);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return texture_array;
}

void opengltf::Model::loadTextures(tinygltf::Model& model)
{
    loadImages(model);
    loadSamplers(model);
    
    for (const tinygltf::Texture& tex : model.textures)
    {
        Texture texture;
        assert(("Invalid texture index"), tex.source != -1);

        texture.name             = tex.name.c_str();
        texture.image            = &images[tex.source];
        texture.image->pixelType = images[tex.source].pixelType;

        if (tex.sampler != -1)
        {
            texture.sampler = &samplers[tex.sampler];
        }
        
        // Set texel format
        switch (images[tex.source].nbChannels)
        {
            case 1:  texture.image->nbChannels = GL_RED; break;
            case 2:  texture.image->nbChannels = GL_RG; break;
            case 3:  texture.image->nbChannels = GL_RGB; break;
            case 4:  texture.image->nbChannels = GL_RGBA; break;
            default: texture.image->nbChannels = GL_RGBA;
        }

        textures.push_back(texture);
    }

    std::cerr << "Loaded: " << textures.size() << " textures." << std::endl;

}

void opengltf::Model::loadImages(tinygltf::Model& model)
{
    for (const tinygltf::Image& im : model.images)
    {
        images.push_back({ im.width, im.height, im.bits, im.component, im.pixel_type, im.uri.c_str(), im.image});
    }

    std::cerr << "Loaded: " << images.size() << " images." << std::endl;

}

void opengltf::Model::loadSamplers(tinygltf::Model& model)
{
    for (const tinygltf::Sampler& smp : model.samplers)
    {
        samplers.push_back(Sampler(smp.minFilter, smp.magFilter, smp.wrapS, smp.wrapT));
    }

    std::cerr << "Loaded: " << samplers.size() << " samplers." << std::endl;
}

glm::mat4 opengltf::Node::getLocalTransform() const
{
    return ( glm::translate(glm::mat4(1.0), translation) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0), scale) ) * matrix;
}

/// <summary>
/// 
/// </summary>
/// <returns> Global transform of the node </returns>
glm::mat4 opengltf::Node::getGlobalTransform() 
{
    // The global transform of a node is given by the product of all local transforms on the path from the root to the respective node. (glTF 2.0 Quick Reference Guide)
    glm::mat4 globalTransform = getLocalTransform();
    
    while(parent)
    {
        globalTransform = globalTransform * parent->getLocalTransform();
        parent = parent->parent;
    }

    return globalTransform;
}

opengltf::Model::~Model()
{
    std::cerr << "Destructor : Model\n";
    glDeleteBuffers(1, &textureArray);
}