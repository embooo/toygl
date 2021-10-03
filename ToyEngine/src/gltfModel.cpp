#pragma once

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "gltfModel.h"
#include <glm/gtc/type_ptr.hpp>

void glTFImporter::Model::draw(Shader& shader)
{
    m_VBO.bind();
    m_VAO.bind();

    //glDrawArrays(GL_LINE, 0, totalNumVertices);

    for(Node* node : nodes)
    {
        drawRecursively(node, shader);
    }
}

void glTFImporter::Model::drawRecursively(Node* node, Shader& shader)
{
    if (node->mesh)
    {
        drawMeshPrimitives(node, shader);
    }

    if (!node->children.empty())
    {
        for (Node* child : node->children)
        {
            drawRecursively(child, shader);
        }
    }
}


void glTFImporter::Model::loadFromFile(const std::string& filename)
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

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        loadImages(tinyglTFModel);
        loadSamplers(tinyglTFModel);
        loadTextures(tinyglTFModel);

        
        /////// 1. Extract model data from nodes 
        for(const tinygltf::Scene& scene : tinyglTFModel.scenes)
        {
            for(int nodeId : scene.nodes)
            {
               const tinygltf::Node& node = tinyglTFModel.nodes.at(nodeId);

               traverseNode(tinyglTFModel, vertices, indices, nodeId, nullptr, node);
            }
        }

        std::cerr << "Loaded: " << materials.size() << " materials." << std::endl;

        /////// 2. Setup OpenGL buffers
        createTextureBuffers();

        // Create Vertex array object
        // Storing layout, format of vertex data and buffer objects
        m_VAO.create();
        m_VAO.bind();

        // Create Vertex buffer object
        // Stores vertex data
        m_VBO.create();
        m_VBO.bind();
        m_VBO.initData(sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        //// Create Element buffer object
        //// Stores vertex indices
        m_EBO.create();
        m_EBO.bind();
        m_EBO.initData(indices, GL_UNSIGNED_SHORT, GL_STATIC_DRAW);

        // Here we specify the layout and format of the vertex data 
        // Stride is the amount of bytes between consecutive vertex attributes of the same kind
        // Vertex attributes are interleaved so we need to specify a stride != 0
        const size_t stride = sizeof(Vertex);

        // 0, 1, 2, 3 are indices for Position, Normal, Texcoord and Color respectively
        m_VAO.setupAttribute(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        m_VAO.setupAttribute(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
        m_VAO.setupAttribute(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texcoord_0));
        m_VAO.setupAttribute(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texcoord_1));

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

/// <summary>
/// This function converts a glTF node into an internal representation
/// and sends it to be processed.
/// </summary>
/// <param name="model"> Contains glTF data parsed/extracted by tinygltf </param>
/// <param name="parentIndex"> Index of the parent node in the glTF file </param>
/// <param name="parent"> Internal representation of a glTF node </param>
/// <param name="glTFDataNode"> Tinygltf representation of a glTF node </param>
void glTFImporter::Model::traverseNode(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int nodeIndex, glTFImporter::Node* parent, const tinygltf::Node& glTFDataNode)   
{
    glTFImporter::Node* current(new Node{});
    
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


void glTFImporter::Model::processElements(tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glTFImporter::Node* parent, const tinygltf::Node& node, glTFImporter::Node* internalNode)  // processNode
{
    internalNode->name = node.mesh;
    // Extract local transform
    if (node.matrix.size() == 16)
    {
        internalNode->matrix = glm::make_mat4(node.matrix.data());
    }

    if (node.translation.size() == 3) 
    {
        internalNode->translation = glm::make_vec3(node.translation.data());
    }

    if (node.rotation.size() == 4) 
    {
        internalNode->rotation = glm::make_quat(node.rotation.data());
    }

    if (node.scale.size() == 3) 
    {
        internalNode->scale = glm::make_vec3(node.scale.data());
    }

    // Check if node refers to a mesh or a camera
    if (node.mesh != -1)
    {
        internalNode->mesh = new Mesh();
        const tinygltf::Mesh& glTFMesh = model.meshes.at(node.mesh);
        internalNode->mesh->name = glTFMesh.name;

        
        for(const tinygltf::Primitive& p : glTFMesh.primitives)
        {
            Primitive* primitive = new Primitive{};

            primitive->startVertices = vertices.size(); 
            primitive->startIndices  = indices.size();

            if(p.mode > -1) primitive->mode = p.mode;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Extract Vertex Attributes 
            /// 1. Extract the buffers and informations regarding layout

            const float* positionBuffer  = nullptr;

            const float* normalBuffer    = nullptr;

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


            ////////////////////////////////////////////////////////////////////////////////// 
            /// 
            for(size_t i = 0; i < primitive->numVertices; ++i)
            {
                Vertex vertex;
                
                vertex.position   = glm::make_vec3(&positionBuffer [i * 3]);

                if(normalBuffer)    vertex.normal     = glm::make_vec3(&normalBuffer[i * 3]);
                if(texcoord0Buffer) vertex.texcoord_0 = glm::make_vec2(&texcoord0Buffer[i * 2]);
                if(texcoord1Buffer) vertex.texcoord_1 = glm::make_vec2(&texcoord1Buffer[i * 2]);

                // ... other attributes

                vertices.push_back(vertex);
            }

            ////////////////////////////////////////////////////////////////////////////////// Extract Indices
            if(p.indices != -1) 
            {
                const tinygltf::Accessor& accessor     = model.accessors.at(p.indices);
                const tinygltf::BufferView& bufferView = model.bufferViews.at(accessor.bufferView);
                const tinygltf::Buffer& buffer         = model.buffers.at(bufferView.buffer);

                const void* indicesBuffer              = &(buffer.data[bufferView.byteOffset + accessor.byteOffset]); 

                primitive->numIndices   = accessor.count;

                switch(accessor.componentType)
                {
                    // 5125 
                    case GL_UNSIGNED_INT:
                    {
                        primitive->indicesType = GL_UNSIGNED_INT;
                        const unsigned int* bufferIndices = static_cast<const unsigned int*>(indicesBuffer);
                        for (size_t i = 0; i < primitive->numIndices; ++i)
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
                        for (size_t i = 0; i < primitive->numIndices; ++i)
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
                        for (size_t i = 0; i < primitive->numIndices; ++i)
                        {
                            indices.push_back(primitive->startVertices + bufferIndices[i]);
                        }
                        break;
                    }

                    default: std::cerr << "Unknown componentType for indices\n";
                }
            }

            ////////////////////////////////////////////////////////////////////////////////// Extract material ID
            if (p.material != -1)
            {
                primitive->materialID = p.material;

                if (materials.find(p.material) == materials.end())
                {
                    // Add new material
                    loadMaterials(model.materials[p.material], p.material);
                }
            }
            internalNode->mesh->primitives.push_back(primitive);
        }

    }
    else if (node.camera != -1)
    {
        const tinygltf::Camera& camera = model.cameras.at(node.camera);

        internalNode->camera = new Camera(
            camera.perspective.aspectRatio, 
            camera.perspective.yfov, 
            camera.perspective.zfar, 
            camera.perspective.znear
        );

        internalNode->camera->name = camera.name;

    }


    // Node hierarchy
    if (parent)
    {
        internalNode->parent = parent;
        parent->children.push_back(internalNode);
    }
    else
    {
        nodes.push_back(internalNode);
    }
}

void glTFImporter::Model::loadMaterials(tinygltf::Material& material, int materialId)
{
        glTFImporter::Material internalMat;
        
        const tinygltf::PbrMetallicRoughness& pbr = material.pbrMetallicRoughness;

        // https://substance3d.adobe.com/tutorials/courses/the-pbr-guide-part-2
        // PBR metallic-roughness workflow : Base Color + Roughness + Metallic
        // Common to all workflow : Ambient occlusion map, Normal map, Height map
        
        // Base color texture
        if (pbr.baseColorTexture.index != -1)
        {
            internalMat.hasBaseColorTexture           = true;
            internalMat.baseColorTexture              = &textures[pbr.baseColorTexture.index];
            internalMat.baseColorTexture->index       = pbr.baseColorTexture.index;
            internalMat.baseColorFactor               = glm::make_vec4(pbr.baseColorFactor.data());
            internalMat.baseColorTexture->texCoordSet = pbr.baseColorTexture.texCoord;
        }

        // Metallic-Roughness texture
        if (pbr.metallicRoughnessTexture.index != -1)
        {
            internalMat.hasMetallicRoughnessTexture           = true;
            internalMat.metallicRoughnessTexture              = &textures[pbr.metallicRoughnessTexture.index];
            internalMat.metallicRoughnessTexture->index       = pbr.metallicRoughnessTexture.index;
            internalMat.metallicRoughnessTexture->texCoordSet = pbr.metallicRoughnessTexture.texCoord;
            internalMat.roughnessFactor                       = pbr.roughnessFactor;
            internalMat.metallicFactor                        = pbr.metallicFactor;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////// Common to all workflows 
        
        // Emissive 
        if (material.emissiveTexture.index != -1)
        {
            internalMat.hasEmissiveTexture           = true;
            internalMat.emissiveTexture              = &textures[material.emissiveTexture.index];
            internalMat.emissiveTexture->index       = material.emissiveTexture.index;
            internalMat.emissiveTexture->texCoordSet = material.emissiveTexture.texCoord;
            internalMat.emissiveFactor               = glm::make_vec3(material.emissiveFactor.data());
        }

        // Normal map
        if (material.normalTexture.index != -1)
        {
            internalMat.hasNormalTexture = true;
            internalMat.normalTexture = &textures[material.normalTexture.index];
            internalMat.normalTexture->index = material.normalTexture.index;
            internalMat.normalTexture->texCoordSet = material.normalTexture.texCoord;
        }

        materials.insert(std::pair<int, Material>(materialId, internalMat)) ;
}



void glTFImporter::Model::drawMeshPrimitives(Node* node, Shader& shader)
{
    shader.setMat4("model", node->getGlobalTransform());

    for (const Primitive* p : node->mesh->primitives)
    {
        const glTFImporter::Material& material = materials.at(p->materialID);
        if (material.hasBaseColorTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, materials.at(p->materialID).baseColorTexture->glBufferId);
            shader.setInt("baseColorTexture", 0); // set texture's unit, not texture's id !
            shader.setInt("currentBaseColorTexcoord", materials.at(p->materialID).baseColorTexture->texCoordSet);
        }

        if (material.hasNormalTexture)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, materials.at(p->materialID).normalTexture->glBufferId);
            shader.setInt("normalTexture", 1);
            shader.setInt("currentNormalTexcoord", materials.at(p->materialID).normalTexture->texCoordSet);
        }

        //if (material.hasMetallicRoughnessTexture)
        //{
        //    shader.setInt("metallicRoughnessTexture",         materials.at(p->materialID).metallicRoughnessTexture->glBufferId);
        //    shader.setFloat("metallicFactor",                 materials.at(p->materialID).metallicFactor);
        //    shader.setFloat("roughnessFactor",                materials.at(p->materialID).roughnessFactor);
        //    shader.setInt("currentMetallicRoughnessTexcoord", materials.at(p->materialID).metallicRoughnessTexture->texCoordSet);
        //}

        //if (material.hasEmissiveTexture)
        //{
        //    shader.setInt("emissiveTexture",   materials.at(p->materialID).emissiveTexture->glBufferId);
        //    shader.setFloat3("emissiveFactor", materials.at(p->materialID).emissiveFactor);
        //}


        glDrawElements(p->mode, p->numIndices, GL_UNSIGNED_INT, (void*)(p->startIndices * sizeof(GL_UNSIGNED_INT)));
    }
}


void glTFImporter::Model::createTextureBuffers()
{
    std::vector<GLuint> texIds;
    texIds.resize(materials.size());

    GLuint sampler;
    glGenSamplers(1, &sampler);

    if (samplers.size() > 0)
    {
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, samplers[0].minFilter);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, samplers[0].magFilter);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, samplers[0].wrapS);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, samplers[0].wrapT);
    }
    else
    {
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }

    for (std::unordered_map<int, Material>::iterator it = materials.begin(); it != materials.end(); ++it)
    {
        // Load base color texture
        const glTFImporter::Material& mat = it->second;

        if (mat.hasBaseColorTexture)
        {
            glActiveTexture(GL_TEXTURE0);

            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            
            glBindSampler(GL_TEXTURE0, sampler);
            GLuint format = mat.baseColorTexture->image->channels == 3 ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D,
                /* level */ 0,
                /* texel format */ format,
                /* width, height, border */ mat.baseColorTexture->image->width, mat.baseColorTexture->image->height, 0,
                /* data format */ format, /* data type */ mat.baseColorTexture->image->pixel_type,
                /* data */ mat.baseColorTexture->image->imData.data());
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

            mat.baseColorTexture->glBufferId = id;
            textureBufferIds.push_back(id);
            glGenerateMipmap(GL_TEXTURE_2D);

        }

        if (mat.hasNormalTexture)
        {
            GLuint id;
            glGenTextures(1, &id);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, id);

            glBindSampler(GL_TEXTURE1, sampler);
            GLuint format = mat.normalTexture->image->channels == 3 ? GL_RGB : GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D,
                /* level */ 0,
                /* texel format */ format,
                /* width, height, border */ mat.normalTexture->image->width, mat.normalTexture->image->height, 0,
                /* data format */ format, /* data type */ mat.normalTexture->image->pixel_type,
                /* data */ mat.normalTexture->image->imData.data());
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            mat.normalTexture->glBufferId = id;
            textureBufferIds.push_back(id);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        //if (mat.hasMetallicRoughnessTexture)
        //{
        //    GLuint id;
        //    glGenTextures(1, &id);

        //    glActiveTexture(GL_TEXTURE2);
        //    glBindTexture(GL_TEXTURE_2D, id);
        //    glBindSampler(GL_TEXTURE2, sampler);

        //    glTexImage2D(GL_TEXTURE_2D,
        //        /* level */ 0,
        //        /* texel format */ GL_RGBA,
        //        /* width, height, border */ mat.metallicRoughnessTexture->image->width, mat.metallicRoughnessTexture->image->height, 0,
        //        /* data format */ GL_RGBA, /* data type */ mat.metallicRoughnessTexture->image->pixel_type,
        //        /* data */ mat.metallicRoughnessTexture->image->imData.data());
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        //    mat.normalTexture->glBufferId = id;
        //    textureBufferIds.push_back(id);
        //    glGenerateMipmap(GL_TEXTURE_2D);

        //}


    }
}


void glTFImporter::Model::loadTextures(tinygltf::Model& model)
{
    for (const tinygltf::Texture& tex : model.textures)
    {
        Texture newTexture;
        assert(("Invalid texture index"), tex.source != -1);

        newTexture.name     = tex.name;
        newTexture.image    = &images[tex.source];
        
        if (tex.sampler != -1)
        {
            newTexture.sampler = &samplers[tex.sampler];
        }

        textures.push_back(newTexture);
    }

    std::cerr << "Loaded: " << textures.size() << " textures." << std::endl;

}

void glTFImporter::Model::loadImages(tinygltf::Model& model)
{
    for (const tinygltf::Image& im : model.images)
    {
        images.push_back( glTFImporter::Image(im.uri, im.width, im.height, im.bits, im.component, im.pixel_type, im.image) );
    }

    std::cerr << "Loaded: " << images.size() << " images." << std::endl;

}


void glTFImporter::Model::loadSamplers(tinygltf::Model& model)
{
    for (const tinygltf::Sampler& smp : model.samplers)
    {
        samplers.push_back(Sampler(smp.minFilter, smp.magFilter, smp.wrapS, smp.wrapT));
    }

    std::cerr << "Loaded: " << samplers.size() << " samplers." << std::endl;
}

glm::mat4 glTFImporter::Node::getLocalTransform()
{
    return ( glm::translate(glm::mat4(1.0), translation) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0), scale) ) * matrix;
}

/// <summary>
/// 
/// </summary>
/// <returns> Global transform of the node </returns>
glm::mat4 glTFImporter::Node::getGlobalTransform()
{
    // The global transform of a node is given by the product of all local transforms on the path from the root to the respective node. (glTF 2.0 Quick Reference Guide)
    glm::mat4 globalTransform = getLocalTransform();
    
    while(parent)
    {
        globalTransform = globalTransform * parent->matrix;
        parent = parent->parent;
    }

    return globalTransform;
}

glTFImporter::Model::~Model()
{
    glDeleteBuffers(1, textureBufferIds.data());
}