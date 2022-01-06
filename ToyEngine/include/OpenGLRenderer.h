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
    void setClearColor(float r, float g, float b, float a = 1.0f);
    void clear();

private:
    glm::vec4 clearColor;
};