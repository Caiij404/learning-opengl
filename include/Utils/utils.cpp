#include "utils.h"
#include <glad/glad.h>
#include <iostream>
using namespace std;


void compileInfoLog(unsigned int id, logType type)
{
    int success;
    char infoLog[512];
    if (type == logType::SHADER)
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }
    else if (type == logType::SHADER_PROGRAM)
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADERPROGRAM::LINK::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }
}