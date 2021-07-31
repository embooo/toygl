#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    //Delete the copy constructor/assignment.
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#The_Object_Oriented_Language_Problem
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader& operator=(Shader&& other);
    ~Shader();

    void use() const;
    void release();

private:
    GLuint m_Id = 0;
    static std::string readFile(const std::string& path);
    static std::string glShaderTypeToString(GLenum shaderType);
    static GLint checkShaderCompileSuccess(GLenum shaderType, GLuint shader);
    static GLint checkShaderProgramLinkSuccess(GLuint program);
    int buildShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
};