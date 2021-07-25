#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath);

    void use() const;

private:
    GLuint m_programID;
    std::string readFile(const std::string path) const;
    GLint checkCompileSuccess(GLuint shaderIndex) const;
    GLint checkLinkSuccess(GLuint program) const;
    int buildShaderProgram(const std::string vertexPath, const std::string fragmentPath);
};