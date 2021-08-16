#pragma once

#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer()
    : clearColor(glm::vec4(0.1f, 0.1f, 0.1f, 0.1f))
{
}

void OpenGLRenderer::init()
{
    glEnable(GL_CULL_FACE);  
    glEnable(GL_DEPTH);  
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

void OpenGLRenderer::setViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


