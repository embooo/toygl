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

struct RenderStats
{
    uint32_t numDrawCalls;
    uint32_t numIndices;
    uint32_t numVertices;

    void reset() { memset(this, 0, sizeof(*this)); }
};


class OpenGLRenderer
{
public:
    OpenGLRenderer();

    void init();
    void setViewport(int width, int height);
    void setClearColor(float r, float g, float b, float a = 1.0f);

    void render(opengltf::Model& model, Shader& program, Camera& camera, Light& light);

    void buildDrawIndirectCommands(opengltf::Model& model);
    void buildDrawIndirectCommands(opengltf::Model& model, opengltf::Node* curNode, bool firstCall = true);

    RenderStats statistics;

    glm::vec3 translation = glm::vec3(0.0f), scale = glm::vec3(1.0f);
    bool m_bEnableMultiDrawIndirect;
    bool m_bEnableNormalMap, m_bEnableGammaCorrection;
private:
    glm::vec4 clearColor;
    std::vector<IndirectParam> drawIndirectCommands;
    unsigned int drawIndirectBuffer;

    void drawNode(opengltf::Node* node, Shader& program, opengltf::Model& model);

};