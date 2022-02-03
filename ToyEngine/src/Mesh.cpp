#include "PCH.h"

#include "Mesh.h"

Mesh::Mesh() : m_VertexArray(0), m_VertexBuffer(0), m_ElementBuffer(0)
{

}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    //m_Vertices = vertices;
    //m_Indices  = indices;

    //setup();
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
    printf("Destructor : Mesh\n");
}

/// <summary>
/// 
/// </summary>
void Mesh::setup()
{
    // Create Vertex array object
    // Storing layout, format of vertex data and buffer objects
    m_VAO.generate();
    m_VAO.bind();

    // Create Vertex buffer object
    // Stores vertex data
    m_VBO.generate();
    m_VBO.bind();
    m_VBO.initData(sizeof(Vertex) * m_Vertices.size(), m_Vertices.data(), GL_STATIC_DRAW);

    // Create Element buffer object
    // Stores vertex indices
    m_EBO.generate();
    m_EBO.bind();
    m_EBO.initData(m_Indices, m_EBO.getComponentType(), GL_STATIC_DRAW);

    // Here we specify the layout and format of the vertex data 
    // Stride is the amount of bytes between consecutive vertex attributes of the same kind
    // Vertex attributes are interleaved so we need to specify a stride != 0
    const size_t stride = sizeof(Vertex);

    // 0, 1, 2, 3 are indices for Position, Normal, Texcoord and Color respectively
    m_VAO.setupAttribute(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    m_VAO.setupAttribute(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
    m_VAO.setupAttribute(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texcoord));
    m_VAO.setupAttribute(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, color));

    // Finished modifying our buffers and array
    m_VAO.unbind();
    m_VBO.unbind();
    m_EBO.unbind();
}

void Mesh::loadglTF(const std::string& filename)
{
}

void Mesh::setupGltf()
{
    //if(gltfLoader.loadSuccess)
    //{
    //    tinygltf::Model model = gltfLoader.extractModel();

    //    Attribute indices = gltfLoader.extractIndices<unsigned short>(model, 0, 0);
    //    m_Indices = std::move(indices.data);
    //    m_EBO.setComponentType(indices.componentType);

    //    Position position   = gltfLoader.extractAttribute<glm::vec3>("POSITION");
    //    Attribute texcoord_0 = gltfLoader.extractAttribute<glm::vec2>("TEXCOORD_0");

    //    m_Vertices.resize(position.data.size());

    //    for(int i=0; i < m_Vertices.size(); ++i)
    //    {
    //        m_Vertices.at(i).position = position.data.at(i);
    //        m_Vertices.at(i).texcoord = texcoord_0.data.at(i);
    //    }

    //    m_DefaultDrawMode = gltfLoader.extractDrawMode(model, 0, 0);

    //    setup();
    //}
    //else
    //{
    //    std::cerr << "Cannot setup mesh from gltf file. \n";
    //}
}


//void Mesh::setupGltf()
//{
//    const tinygltf::Model& model = gltfLoader.getModel();
//    
//    m_Indices = gltfLoader.extractIndices(model, 0, 0);
//
//    std::cout << m_Indices.size();
//}

void Mesh::draw()
{

}

void Mesh::drawIndexed()
{
    m_VAO.bind();
    glDrawElements(m_DefaultDrawMode, m_EBO.getNumIndices(), m_EBO.getComponentType(), 0);
}

void Mesh::release()
{
    m_VAO.release();
    m_VBO.release();
    m_EBO.release();
}
