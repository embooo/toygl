#include "PCH.h"

#include "OpenGLBuffers.h"

VertexArray::VertexArray()
    : m_Id(0)
{
}

void VertexArray::generate()
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

void VertexArray::setupAttribute(int index, int size, GLenum type, bool normalized, size_t stride, const void *offset)
{
    glEnableVertexAttribArray(index);
    if (type == GL_INT || type == GL_UNSIGNED_INT)
    {
        glVertexAttribIPointer(index, size /* nb of components */, type /* components type */, stride, offset);
    }
    else
    {
        glVertexAttribPointer(index, size /* nb of components */, type /* components type */, normalized, stride, offset);
    }
}

VertexArray::~VertexArray()
{
    printf("Destructor : VertexArray\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer()
    : m_Id(0), m_Size(0)
{

}

void VertexBuffer::generate()
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
    printf("Destructor : VertexBuffer\n");
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

void ElementBuffer::generate()
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
    printf("Destructor : ElementBuffer\n");

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

SSBO::SSBO()
    : m_Id(0)
{
}

void SSBO::generate()
{
    glGenBuffers(1, &m_Id);
}

void SSBO::setData(size_t size, const void* data, GLenum usage)
{
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
}

void SSBO::bind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Id);
}

void SSBO::bindBase(int bindingPoint)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_Id);
}

void SSBO::unbind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::release()
{
    glDeleteBuffers(1, &m_Id);
}

SSBO::~SSBO()
{
    printf("Destructor : SSBO\n");
}
