#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

// This class represents an OpenGL mesh

struct Vertex
{
    Vertex(const glm::vec3 position)
        : position(position), normal(), texcoord(), color() {}
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec4 color;
};

class Mesh
{
public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    void setup();
    void draw(const Shader& shader);
    void release();
private:
    GLuint m_VertexArray;
    GLuint m_VertexBuffer;
    GLuint m_ElementBuffer; // Also known as Index buffer

    std::vector<Vertex> m_Vertices;
    std::vector<GLuint> m_Indices;
};