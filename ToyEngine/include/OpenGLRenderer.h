#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>  

#include "shader.h"
#include "gltfModel.h"
#include "Camera.h"
#include "Light.h"


class OpenGLRenderer
{
public:
    OpenGLRenderer();

    void init();
    void setViewport(int width, int height);
    void setClearColor(float r, float g, float b, float a = 1.0f);
    void clear();

    // gltf specific
    void render(gltf::Model& model, Shader& program, Camera& camera, Light& light);

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
private:
    glm::vec4 clearColor;

    // gltf specific
    void drawNode(gltf::Node* node, Shader& program, gltf::Model& model);

};