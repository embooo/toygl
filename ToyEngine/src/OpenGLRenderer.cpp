#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer()
    : clearColor(glm::vec4(0.2, 0.2, 0.2, 1))
{
}

void OpenGLRenderer::init()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClearDepthf(1.f);

    //glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW); 
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); 
}

void OpenGLRenderer::render(gltf::Model& model, Shader& program, Camera& camera, Light& light)
{
    program.use();

    model.m_VBO.bind();
    model.m_VAO.bind();

    statistics.reset();
    
    //program.setMat4("model",      glm::translate(glm::identity<glm::mat4>(), translation));
    program.setMat4("view",       camera.viewMat());
    program.setMat4("projection", camera.projMat());

    program.setFloat3("cameraPos",  camera.pos());
    program.setFloat3("lightPos",   light.pos());
    program.setFloat4("lightColor", light.color());

    program.setFloat("lightRadius", ((PointLight&)light).radius());

    for (gltf::Node* node : model.nodes)
    {
        if (node->mesh)
        {
            drawNode(node, program, model);
        }

        if (!node->children.empty())
        {
            for (gltf::Node* node : node->children)
            {
                if (node->mesh)
                {
                    drawNode(node, program, model);
                }
            }
        }
    }
}

void OpenGLRenderer::drawNode(gltf::Node* node, Shader& program, gltf::Model& model)
{
    program.setMat4("model", node->getGlobalTransform() * glm::scale(scale) * glm::translate(translation));

    for (const gltf::Primitive* p : node->mesh->primitives)
    {
        const gltf::Material& material = model.materials.at(p->materialID);
        if (material.hasBaseColorTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.baseColorTexture->glBufferId);
            program.setInt("baseColorTexture", 0); // set texture unit's ID, not the texture buffer's id !
            program.setInt("currentBaseColorTexcoord", material.baseColorTexture->texCoordSet);
        }

        if (material.hasNormalTexture)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.normalTexture->glBufferId);
            program.setInt("normalTexture", 1);
            program.setInt("currentNormalTexcoord", material.normalTexture->texCoordSet);
        }

        if (material.hasMetallicRoughnessTexture)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material.metallicRoughnessTexture->glBufferId);
            program.setInt("metallicRoughnessTexture", 2);
            program.setFloat("metallicFactor", material.metallicFactor);
            program.setFloat("roughnessFactor", material.roughnessFactor);
            program.setInt("currentMetallicRoughnessTexcoord", material.metallicRoughnessTexture->texCoordSet);
        }

        if (material.hasEmissiveTexture)
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, material.emissiveTexture->glBufferId);
            program.setInt("emissiveTexture", 3);
            program.setFloat3("emissiveFactor", material.emissiveFactor);
        }

        glDrawElements(p->mode, p->numIndices, GL_UNSIGNED_INT, (void*)(p->startIndices * sizeof(GL_UNSIGNED_INT)));
        
        statistics.numDrawCalls += 1;
        statistics.numIndices   += p->numIndices;
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


