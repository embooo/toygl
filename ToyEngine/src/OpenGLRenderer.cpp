#include "PCH.h"

#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer()
    : clearColor(glm::vec4(0.2, 0.2, 0.2, 1)), m_bEnableMultiDrawIndirect(true), m_bEnableNormalMap(true), m_bEnableGammaCorrection(true)
{
}

#ifdef DEBUG_BUILD
    void GLAPIENTRY
    MessageCallback(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam)
    {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
    }
#endif

void OpenGLRenderer::init()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClearDepthf(1.f);

    //glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW); 
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); 

#ifdef DEBUG_BUILD
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
#endif
}

void OpenGLRenderer::render(opengltf::Model& model, Shader& program, Camera& camera, Light& light)
{
    program.use();

    model.m_VAO.bind();

    statistics.reset();
    
    program.setMat4("model",      model.modelMat() * glm::scale(scale) * glm::translate(translation));
    //program.setMat4("model",      glm::scale(scale) * glm::translate(translation));
    program.setMat4("view",       camera.viewMat());
    program.setMat4("projection", camera.projMat());

    program.setFloat3("cameraPos",  camera.pos());
    program.setInt("enableNormalMap", m_bEnableNormalMap);
    program.setInt("enableGammaCorrection", m_bEnableGammaCorrection);

    if (light.type() == LightType::POINT_LIGHT)
    {
        program.setFloat3("lightPos", light.pos());
        program.setFloat4("lightColor", light.color());

        program.setFloat("lightRadius", ((PointLight&)light).radius());
    }

    if (light.type() == LightType::DIR_LIGHT)
    {
        program.setFloat3("lightDir",   ((DirectionalLight&)light).dir());
        program.setFloat4("lightColor", light.color());
    }

    if (m_bEnableMultiDrawIndirect)
    {
        // Configure the texture unit pointing to the 3D texture array
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, model.textureArray);
        program.setInt("textureArray", 0);

        // Bind buffer containing all material informations
        model.ssbo_materials.bind();

        // Bind buffer containing all draw commands for the indirect draw
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndirectBuffer);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, drawIndirectCommands.size(), 0);

        statistics.numDrawCalls += 1;
        statistics.numIndices   += model.indices.size();
        statistics.numVertices  += model.vertices.size();
    }
    else
    {
        for (opengltf::Node* node : model.nodes)
        {
            if (node->mesh)
            {
                drawNode(node, program, model);
            }

            if (!node->children.empty())
            {
                for (opengltf::Node* node : node->children)
                {
                    if (node->mesh)
                    {
                        drawNode(node, program, model);
                    }
                }
            }
        }
    }

}

void OpenGLRenderer::buildDrawIndirectCommands(opengltf::Model& model)
{
    for (opengltf::Node* node : model.nodes)
    {
        if (node->mesh)
        {
            for (const opengltf::Primitive* p : node->mesh->primitives)
            {
                drawIndirectCommands.push_back({ p->indexCount, 1, p->firstIndex, 0, 0 });
            }
        }

        if (!node->children.empty())
        {
            for (opengltf::Node* node : node->children)
            {
                if (node->mesh)
                {
                    for (const opengltf::Primitive* p : node->mesh->primitives)
                    {
                        drawIndirectCommands.push_back({ p->indexCount, 1, p->firstIndex, 0, 0 });
                    }
                }
            }
        }
    }

    // Generate OpenGL buffer
    glGenBuffers(1, &drawIndirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectParam) * drawIndirectCommands.size(), drawIndirectCommands.data(), GL_DYNAMIC_DRAW);
}

void OpenGLRenderer::drawNode(opengltf::Node* node, Shader& program, opengltf::Model& model)
{
    for (const opengltf::Primitive* p : node->mesh->primitives)
    {
        const opengltf::Material& material = model.materials.at(p->materialID);
        //if (material.hasBaseColorTexture)
        //{
        //    glActiveTexture(GL_TEXTURE0);
        //    glBindTexture(GL_TEXTURE_2D, material.baseColorTexture->glBufferId);
        //    program.setInt("baseColorTexture", 0); // set texture unit's ID, not the texture buffer's id !
        //    program.setInt("currentBaseColorTexcoord", material.baseColorTexture->texCoordSet);
        //}

        //if (material.hasNormalTexture)
        //{
        //    glActiveTexture(GL_TEXTURE1);
        //    glBindTexture(GL_TEXTURE_2D, material.normalTexture->glBufferId);
        //    program.setInt("normalTexture", 1);
        //    program.setInt("currentNormalTexcoord", material.normalTexture->texCoordSet);
        //}

        //if (material.hasMetallicRoughnessTexture)
        //{
        //    glActiveTexture(GL_TEXTURE2);
        //    glBindTexture(GL_TEXTURE_2D, material.metallicRoughnessTexture->glBufferId);
        //    program.setInt("metallicRoughnessTexture", 2);
        //    program.setFloat("metallicFactor", material.metallicFactor);
        //    program.setFloat("roughnessFactor", material.roughnessFactor);
        //    program.setInt("currentMetallicRoughnessTexcoord", material.metallicRoughnessTexture->texCoordSet);
        //}

        //if (material.hasEmissiveTexture)
        //{
        //    glActiveTexture(GL_TEXTURE3);
        //    glBindTexture(GL_TEXTURE_2D, material.emissiveTexture->glBufferId);
        //    program.setInt("emissiveTexture", 3);
        //    program.setFloat3("emissiveFactor", material.emissiveFactor);
        //}

        glDrawElements(p->mode, p->indexCount, GL_UNSIGNED_INT, (void*)(p->firstIndex * sizeof(GL_UNSIGNED_INT)));
        
        statistics.numDrawCalls += 1;
        statistics.numIndices   += p->indexCount;
        statistics.numVertices  += p->numVertices;
    }
}

void OpenGLRenderer::setViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::setClearColor(float r, float g, float b, float a)
{
    clearColor.r = r;
    clearColor.g = g;
    clearColor.b = b;
    clearColor.a = a;
}

void OpenGLRenderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


