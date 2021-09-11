#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "OpenGLBuffers.h"
#include "Shader.h"

// This class represents an OpenGL mesh

struct Vertex
{
    Vertex()
        : position(position), normal(), texcoord(), color() {};
    Vertex(const glm::vec3 position)
        : position(position), normal(), texcoord(), color() {}

    Vertex& addPosition(const glm::vec3& other) { position = other; return *this; }
    Vertex& addColor(const glm::vec3& other) { color = other; return *this; }
    Vertex& addNormal(const glm::vec3& other) { normal = other; return *this; }
    Vertex& addTexcoord(const glm::vec3& other) { texcoord = other; return *this; }

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
    void drawIndexed();
    void release();
   
    void loadglTF(const std::string& filename);
    void setupGltf();

    
private:
    VertexArray   m_VAO;
    VertexBuffer  m_VBO;
    ElementBuffer m_EBO;

    GLenum m_DefaultDrawMode; // Draw mode

    GLuint m_VertexArray;
    GLuint m_VertexBuffer;
    GLuint m_ElementBuffer; // Also known as Index buffer

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned short> m_Indices;
    
};


