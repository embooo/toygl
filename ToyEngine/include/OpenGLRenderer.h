#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>  

#include "shader.h"
#include "gltfModel.h"
#include "Camera.h"
#include "Light.h"

// Parameters for MultiDrawElementsIndirect
struct IndirectParam
{
    unsigned int indexCount;
    unsigned int instanceCount;
    unsigned int firstIndex;       // offset, same as in glDrawElements()
    unsigned int vertexBase;
    unsigned int instanceBase;
};


class OpenGLRenderer
{
public:
    OpenGLRenderer();

    void init();
    void setViewport(int width, int height);
    void setClearColor(float r, float g, float b, float a = 1.0f);
    void clear();

    // gltf specific
    void render(opengltf::Model& model, Shader& program, Camera& camera, Light& light);

    void buildDrawIndirectCommands(opengltf::Model& model);

    struct RenderStats
    {
        uint32_t numDrawCalls = 0;
        uint32_t numIndices   = 0;
        uint32_t numVertices  = 0;

        void reset() { memset(this, 0, sizeof(*this)); }
    };

    RenderStats statistics;

    // todo : remove 
    glm::vec3 translation = glm::vec3(0.0f), scale = glm::vec3(1.0f);
    bool m_bEnableMultiDrawIndirect;
    bool m_bEnableNormalMap;
private:
    glm::vec4 clearColor;
    std::vector<IndirectParam> drawIndirectCommands;
    unsigned int drawIndirectBuffer;
    // gltf specific
    void drawNode(opengltf::Node* node, Shader& program, opengltf::Model& model);

};