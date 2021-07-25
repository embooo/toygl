#include "..\include\shader.h"

#include <iostream>
#include <sstream>
#include <fstream>

Shader::Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath)
{
    // Has to be called after initializing an OpenGL context

    buildShaderProgram(vertexShaderPath, fragmentShaderPath);

}

void Shader::use() const
{
    glUseProgram(m_programID);
}

std::string Shader::readFile(const std::string path) const
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    std::string str;

    if(file)
    {
        // Get file length in characters
        file.seekg(0, file.end); // Placing indicator at the end with a 0 offset
        size_t length = file.tellg();

        if (length != -1)
        {
            str.resize(length);
            file.seekg(0, file.beg); // Placing indicator at the beginning with a 0 offset
            file.read(&str[0], length);
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

GLint Shader::checkCompileSuccess(GLuint shader) const
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

        std::cout << "Shader compilation encoutered the following errors : \n" << errorLog << ".\n";

        glDeleteShader(shader);
    }

    return compileStatus;
}

GLint Shader::checkLinkSuccess(GLuint program) const
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

        std::cout << "Shader program linkage encoutered the following errors : " << errorLog << ".\n";

        glDeleteProgram(program);
    }

    return linkStatus;
}

int Shader::buildShaderProgram(const std::string vertexPath, const std::string fragmentPath) 
{
    // This function reads and compiles the shader source code
    // specified and links them to create a shader program

    std::string vertexShaderFile = readFile(vertexPath);
    std::string fragmentShaderFile = readFile(fragmentPath);

    if (!vertexShaderFile.empty() && !fragmentShaderFile.empty())
    {
        const char* vertexShaderC = vertexShaderFile.c_str();
        const char* fragmentShaderC = fragmentShaderFile.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderC, NULL);
        glCompileShader(vertexShader);

        if (checkCompileSuccess(vertexShader))
        {
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderC, NULL);
            glCompileShader(fragmentShader);


            if (checkCompileSuccess(fragmentShader))
            {
                // Create the shader program by linking the shaders 
                m_programID = glCreateProgram();

                // Specify the shaders that are to be linked
                glAttachShader(m_programID, vertexShader);
                glAttachShader(m_programID, fragmentShader);

                // Link the shaders to create the complete shader program
                glLinkProgram(m_programID);

                if (checkLinkSuccess(m_programID))
                {
                    // Shaders are now linked to the program
                    // We can delete them
                    glDeleteShader(vertexShader);
                    glDeleteShader(fragmentShader);
                }
            }

        }

        return 0;
    }

    return -1;
}

