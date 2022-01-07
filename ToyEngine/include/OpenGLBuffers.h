#pragma once

#include <glad/glad.h>
#include <vector>

class Buffer
{
public:
    virtual void create() = 0;
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void release() = 0;
    virtual ~Buffer() {};
};
/// <summary>
/// Class representing an OpenGL Vertex Array Object 
/// </summary>
class VertexArray : public Buffer
{
public:
    VertexArray();

    virtual void create() override;
    virtual void bind() const override;
    virtual void unbind() const override;
    virtual void release() override;

    virtual void setupAttribute(int attributeId, int nbComponents, GLenum type, bool normalized, size_t stride, const void * offset);

    virtual ~VertexArray() override;
private:
    unsigned int m_Id;
};

/// <summary>
/// Class representing an OpenGL Vertex Buffer Object
/// </summary>
class VertexBuffer : public Buffer
{
public:
    VertexBuffer();

    virtual void create() override;
    virtual void initData(size_t size, const void* data, GLenum usage = GL_STATIC_DRAW);
    virtual void bind() const override;
    virtual void unbind() const override;
    virtual void release() override;

    virtual ~VertexBuffer() override;
private:
    unsigned int m_Id;
    // Size in bytes of the buffer
    size_t m_Size;
};

/// <summary>
/// Class representing an OpenGL Element Buffer Object
/// </summary>
class ElementBuffer : public Buffer
{
public:
    ElementBuffer();

    virtual void create() override;
    
    template<typename T>
    void initData(const std::vector<T> indices, GLenum dataType, GLenum usage = GL_STATIC_DRAW)
    {
        assert(("Element buffer is not currently bound", m_Id != 0));

        m_ComponentType = dataType;
        m_NumIndices = indices.size();
        m_SizeInBytes = sizeof(dataType) * m_NumIndices;

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_SizeInBytes, &indices[0], usage);
    }

   
    void initData(void* indices, GLenum dataType, uint32_t numIndices, GLenum usage = GL_STATIC_DRAW);

    virtual void bind()    const override;
    virtual void unbind()  const override;
    virtual void release() override;

    GLenum getComponentType();
    void   setComponentType(GLenum type);
    size_t getSizeInBytes();
    size_t getNumIndices();

    virtual ~ElementBuffer() override;

private:
    unsigned int m_Id;
    GLenum m_ComponentType;
    size_t m_SizeInBytes;
    size_t m_NumIndices;
};
