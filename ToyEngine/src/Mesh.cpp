#include "Mesh.h"
#include <iostream>
Mesh::Mesh() : m_VertexArray(0), m_VertexBuffer(0), m_ElementBuffer(0)
{

}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    m_Vertices = vertices;
    m_Indices  = indices;

    setup();
}

Mesh::Mesh(Mesh&& other)
{
    m_VertexArray   = other.m_VertexArray;
    m_VertexBuffer  = other.m_VertexBuffer;
    m_ElementBuffer = other.m_ElementBuffer;

    m_Vertices = std::move(other.m_Vertices);
    m_Indices  = std::move(other.m_Indices);
}

Mesh& Mesh::operator=(Mesh&& other)
{
    if(this != &other)
    {
        release();
        m_VertexArray   = other.m_VertexArray;
        m_VertexBuffer  = other.m_VertexBuffer;
        m_ElementBuffer = other.m_ElementBuffer;

        m_Vertices = std::move(other.m_Vertices);
        m_Indices  = std::move(other.m_Indices);
    }

    return *this;
}

Mesh::~Mesh()
{
}

/// <summary>
/// 
/// </summary>
void Mesh::setup()
{
    // Create Vertex array object
    // Storing layout, format of vertex data and buffer objects
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);
    // Create Vertex buffer object
    // Stores vertex data
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW); // GL_STATIC_DRAW hint as this buffer won't be updated later

    // Create Element buffer object
    // Stores vertex indices
    glGenBuffers(1, &m_ElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

    // Here we specify the layout and format of the vertex data 
    // Stride is the amount of bytes between consecutive vertex attributes of the same kind
    // Vertex attributes are interleaved so we need to specify a stride != 0
    const size_t stride = sizeof(Vertex);

    // 0, 1, 2, 3 are indices for Position, Normal, Texcoord and Color respectively
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3 /* number of components */ , GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3 /* number of components */ , GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2 /* number of components */ , GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texcoord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3 /* number of components */ , GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, color));

    // Finished modifying our buffers and array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw()
{
    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::release()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_ElementBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);

    m_VertexBuffer  = 0;
    m_ElementBuffer = 0;
    m_VertexArray   = 0;
}
