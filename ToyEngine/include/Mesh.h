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
    Vertex& addColor(const glm::vec3& other) { color = other; return *this; }
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 color;
};

class Mesh
{
public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

    // Delete the copy constructor/assignment.
    // RAII and hidden destructor calls
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#The_Object_Oriented_Language_Problem
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh (Mesh&& other);
    Mesh& operator=(Mesh&& other);

    ~Mesh();
    void setup();
    void draw();
    void release();
private:
    GLuint m_VertexArray;
    GLuint m_VertexBuffer;
    GLuint m_ElementBuffer; // Also known as Index buffer

    std::vector<Vertex> m_Vertices;
    std::vector<GLuint> m_Indices;
};