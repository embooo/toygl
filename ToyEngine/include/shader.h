#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    void use() const;

private:
    GLuint m_programID;
    static std::string readFile(const std::string& path);
    static GLint checkCompileSuccess(GLuint shader);
    static GLint checkLinkSuccess(GLuint program);
    int buildShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
};