#pragma once

#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer()
    : clearColor(glm::vec4(0.1f, 0.1f, 0.1f, 0.1f))
{
    //glEnable(GL_DEPTH);  
}

void OpenGLRenderer::init()
{
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


