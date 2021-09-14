#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
{
    m_Program = 0;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    build(vertexPath, fragmentPath);
}

Shader& Shader::operator=(Shader&& other)
{
    if(this != &other)
    {
        release();
        std::swap(m_Program, other.m_Program);
    }

    return *this;
}

Shader::~Shader()
{
    release();
}

void Shader::use() const
{
    glUseProgram(m_Program);
}

void Shader::release()
{
    glDeleteProgram(m_Program);
    m_Program = 0;
}

void Shader::setInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniform1f(location, value);
}

void Shader::setFloat2(const std::string& name, const glm::vec2& value)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void Shader::setFloat3(const std::string& name, const glm::vec3& value)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setFloat4(const std::string& name, const glm::vec4& value)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::setMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = glGetUniformLocation(m_Program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string Shader::readFile(const std::string& path) 
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    std::string str;

    if(file)
    {
        // Get file length in characters
        file.seekg(0, file.end); // Placing indicator at the end with a 0 offset
        auto length = file.tellg();

        if (length != -1)
        {
            str.resize(length);
            file.seekg(0, file.beg); // Placing indicator at the beginning with a 0 offset
            file.read(&str[0], length);

            std::cout << path << " read successfully.\n";
        }
        else
        {
            std::cerr << "Could not read " << path << ".\n";
        }
    }
    else
    {   
        std::cerr << "Could not open " << path << ".\n";
    }

    return str;
}

std::string Shader::glShaderTypeToString(GLenum shaderType)
{
    switch(shaderType)
    {
        case GL_VERTEX_SHADER:              return "Vertex Shader";
        case GL_FRAGMENT_SHADER:            return "Fragment Shader"; 
        case GL_GEOMETRY_SHADER:            return "Geometry Shader"; 
        case GL_COMPUTE_SHADER:             return "Compute Shader"; 
        case GL_TESS_CONTROL_SHADER:        return "Tesselation Control Shader"; 
        case GL_TESS_EVALUATION_SHADER:     return "Tesselation Evaluation Shader"; 

        default: return "Unknown shader type."; 
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="shader">Shader to check for compile errors</param>
/// <param name="log"> Contains OpenGL error log if shader failed to compile </param>
/// <returns></returns>
GLint Shader::checkShaderCompileSuccess(GLenum shaderType, GLuint shader)
{
    GLint compileStatus = false;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus)
    {
        // Get log length including null character
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength); 

        // Get the log's content 
        std::string errorLog;
        errorLog.resize(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, &errorLog[0]);

        std::cerr << glShaderTypeToString(shaderType) << " compilation encountered errors : \n" << errorLog << "\n";

        glDeleteShader(shader);
    }

    return compileStatus;
}

/// <summary>
/// 
/// </summary>
/// <param name="program">Shader program to check for link errors</param>
/// <param name="log">String containing the OpenGL error log if program failed to link</param>
/// <returns></returns>
GLint Shader::checkShaderProgramLinkSuccess(GLuint program)
{
    GLint linkStatus = false;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    {
        // Get log length including null character
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength); 

        // Get the log's content 
        std::string errorLog;
        errorLog.resize(logLength);
        glGetShaderInfoLog(program, logLength, &logLength, &errorLog[0]);

        std::cerr << "Shader program linkage encountered errors : \n" << errorLog << "\n";

        glDeleteProgram(program);
    }

    return linkStatus;
}

int Shader::build(const std::string& vertexPath, const std::string& fragmentPath) 
{
    // Has to be called after initializing an OpenGL context
    // This function compiles the shader source codes
    // specified and links them to create a complete shader program
    m_vertexPath = vertexPath;
    m_fragmentPath = fragmentPath;

    const std::string& vertexShaderFile   = readFile(vertexPath);
    const std::string& fragmentShaderFile = readFile(fragmentPath);

    if (!vertexShaderFile.empty() && !fragmentShaderFile.empty())
    {
        const char* vertexShaderC   = vertexShaderFile.c_str();
        const char* fragmentShaderC = fragmentShaderFile.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderC, NULL);
        glCompileShader(vertexShader);

        
        std::string log;

        if (checkShaderCompileSuccess(GL_VERTEX_SHADER, vertexShader))
        {
            std::cout << "Vertex shader compiled successfully (" << vertexPath << ").\n";
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderC, NULL);
            glCompileShader(fragmentShader);


            if (checkShaderCompileSuccess(GL_FRAGMENT_SHADER, fragmentShader))
            {
                std::cout << "Fragment shader compiled successfully (" << fragmentPath << "). \n";
                // Create the shader program by linking the shaders 
                m_Program = glCreateProgram();

                // Specify the shaders that are to be linked
                glAttachShader(m_Program, vertexShader);
                glAttachShader(m_Program, fragmentShader);

                // Link the shaders to create the complete shader program
                glLinkProgram(m_Program);

                if (checkShaderProgramLinkSuccess(m_Program))
                {
                    std::cout << "Shader program built successfully. \n\n";
                    // Shaders are now linked to the program
                    // We can delete them
                    glDetachShader(m_Program, vertexShader);
                    glDetachShader(m_Program, fragmentShader);
                    glDeleteShader(vertexShader);
                    glDeleteShader(fragmentShader);
                }

            }
        }

        return 0;
    }

    std::cerr << "Could not build shader program.\n";

    return -1;
}

int Shader::rebuild()
{
    if (m_vertexPath.empty() || m_fragmentPath.empty())
    {
        build(m_vertexPath, m_fragmentPath);
        return 0;
    }

    else { std::cerr << "Could not rebuild shader. \n";  return -1; }
}