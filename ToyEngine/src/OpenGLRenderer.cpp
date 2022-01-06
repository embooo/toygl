#pragma once

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


