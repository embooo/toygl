#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>  

#include <Window.h>

class OpenGLRenderer
{
public:
    OpenGLRenderer();

    void init();
    void setViewport(int width, int height);
    void clear();

private:
    glm::vec4 clearColor;
    GLbitfield bufferBits;
};