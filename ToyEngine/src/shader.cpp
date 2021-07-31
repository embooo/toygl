#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.h"

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    buildShaderProgram(vertexShaderPath, fragmentShaderPath);
}

Shader& Shader::operator=(Shader&& other)
{
    if(this != &other)
    {
        release();
        std::swap(m_Id, other.m_Id);
    }

    return *this;
}

Shader::~Shader()
{
    release();
}

void Shader::use() const
{
    glUseProgram(m_Id);
}

void Shader::release()
{
    glDeleteProgram(m_Id);
    m_Id = 0;
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
            std::cout << "Could not read " << path << ".\n";
        }
    }
    else
    {   
        std::cout << "Could not open " << path << ".\n";
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

        std::cout << glShaderTypeToString(shaderType) << " compilation encountered errors : \n" << errorLog << "\n";

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

        std::cout << "Shader program linkage encountered errors : \n" << errorLog << "\n";

        glDeleteProgram(program);
    }

    return linkStatus;
}

int Shader::buildShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) 
{
    // Has to be called after initializing an OpenGL context
    // This function reads and compiles the shader source codes
    // specified and links them to create a complete shader program

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
                m_Id = glCreateProgram();

                // Specify the shaders that are to be linked
                glAttachShader(m_Id, vertexShader);
                glAttachShader(m_Id, fragmentShader);

                // Link the shaders to create the complete shader program
                glLinkProgram(m_Id);

                if (checkShaderProgramLinkSuccess(m_Id))
                {
                    std::cout << "Shader program built successfully. \n\n";
                    // Shaders are now linked to the program
                    // We can delete them
                    glDetachShader(m_Id, vertexShader);
                    glDetachShader(m_Id, fragmentShader);
                    glDeleteShader(vertexShader);
                    glDeleteShader(fragmentShader);
                }

            }
        }

        return 0;
    }

    std::cout << "Could not build shader program.\n";

    return -1;
}

