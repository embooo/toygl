#pragma once
#include "PCH.h"

class Shader
{
public:
    Shader();
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Delete the copy constructor/assignment.
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#The_Object_Oriented_Language_Problem
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Move assignement operator
    Shader& operator=(Shader&& other);

    void use() const;
    int build(const std::string& vertexPath, const std::string& fragmentPath);
    int rebuild();
    void release();

    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setFloat2(const std::string& name, const glm::vec2& value);
    void setFloat3(const std::string& name, const glm::vec3& value);
    void setFloat4(const std::string& name, const glm::vec4& value);
    void setMat3(const std::string& name, const glm::mat3& value);
    void setMat4(const std::string& name, const glm::mat4& value);

private:
    GLuint m_Program;
    std::string m_vertexPath;
    std::string m_fragmentPath;
    static std::string readFile(const std::string& path);
    static std::string glShaderTypeToString(GLenum shaderType);
    static GLint checkShaderCompileSuccess(GLenum shaderType, GLuint shader);
    static GLint checkShaderProgramLinkSuccess(GLuint program);
};