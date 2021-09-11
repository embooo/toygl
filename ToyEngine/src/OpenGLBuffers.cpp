#include "OpenGLBuffers.h"
#include <cassert>

VertexArray::VertexArray()
    : m_Id(0)
{
}

void VertexArray::create()
{
    glGenVertexArrays(1, &m_Id);
}

void VertexArray::bind() const
{
    glBindVertexArray(m_Id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::release()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_Id);
}

void VertexArray::setupAttribute(int attributeId, int nbComponents, GLenum componentType, bool normalized, size_t stride, const void * offset)
{
    glEnableVertexAttribArray(attributeId);
    glVertexAttribPointer(attributeId, nbComponents, componentType, normalized, stride, offset);
}

VertexArray::~VertexArray()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer()
    : m_Id(0), m_Size(0)
{

}

void VertexBuffer::create()
{
    glGenBuffers(1, &m_Id);
}

void VertexBuffer::initData(size_t size, const void* data, GLenum usage)
{
    assert(("Vertex buffer is not currently bound", m_Id != 0));

    m_Size = size;
    glBufferData(GL_ARRAY_BUFFER, m_Size, data, usage);
}


void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::release()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_Id);
}

VertexBuffer::~VertexBuffer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ElementBuffer::ElementBuffer()
    : m_Id(0), m_ComponentType(GL_UNSIGNED_INT), m_NumIndices(0), m_SizeInBytes(0)
{
}
void ElementBuffer::initData(void* indices, GLenum dataType, uint32_t numIndices, GLenum usage)
{
    assert(("Element buffer is not currently bound", m_Id != 0));

    m_ComponentType = dataType;
    m_SizeInBytes = sizeof(dataType) * numIndices;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_SizeInBytes, indices, usage);
}

void ElementBuffer::create()
{
    glGenBuffers(1, &m_Id);
}


void ElementBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void ElementBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ElementBuffer::release()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_Id);
}

ElementBuffer::~ElementBuffer()
{
}

GLenum ElementBuffer::getComponentType()
{
    return m_ComponentType;
}

void ElementBuffer::setComponentType(GLenum type)
{
    m_ComponentType = type;
}

size_t ElementBuffer::getSizeInBytes()
{
    return m_SizeInBytes;
}

size_t ElementBuffer::getNumIndices()
{
    return m_NumIndices;
}
